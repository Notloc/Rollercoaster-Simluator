/*	Rollercoaster.c
 *	This module implements the Roller Coaster, the track, track render, and track editing
 */
#include "engine.h"
#include "rollercoaster.h"
#include "primatives.c"
#include "input.h"
#include "camera.h"
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DEFAULT_NUMBER_OF_POINTS 15
#define NUMBER_OF_SUB_SECTIONS 10

#define CONTROL_POINT_MOVEMENT_SPEED 0.1
#define CONTROL_POINT_HEIGHT_STEP 0.25

#define GRAVITY -9.81
#define FRICTION_COEFFICIENT 0.001

#define COASTER_START_SPEED 2.5
#define BOOST_STRENGTH 0.25
#define CHAIN_LIFT_SPEED 1.5

typedef struct {
	Vector3 position;
	int isChain;	
} ControlPoint;

typedef struct {
	float subSectionLength;

	Vector3 subSectionStart;
	Vector3 subSectionEnd;
} TrackSubSection;

typedef struct {
	TrackSubSection subSections[NUMBER_OF_SUB_SECTIONS];
	int isChain;
} TrackSection;

typedef struct {
	Vector3* topVerts;
	Vector3* bottomVerts;
} RailVerts;

//Init
static void generateControlPoints(void);
static void defaultCoaster(void);

//Update
static void takeInput(void);
static void generateTrack(void);
static void generateTrackDisplayList(void);
static void calculateSubSectionLength(TrackSubSection* subSection);
static void moveCoaster(void);

//Drawing
static void drawControlPoints(void);
static void drawFinishedTrack(void);
static void drawTrain(void);

//Construction
static void selectionInput(void);
static void editControlPoint(void);
static void addPoint(void);
static void removePoint(void);
static void allocateMoreControlPoints(void);

static Vector3 qFunction(float u, int i);


static RailVerts leftRail;
static RailVerts rightRail;

typedef enum { Constructing, Generating, Ready } TrackState;
TrackState trackState = Constructing;

ControlPoint* controlPoints = NULL;
int numberOfControlPoints;
int allocatedControlPoints;
int selectedPoint = -1;

TrackSection* trackSections = NULL;


int trackList;


Vector3 up;

Vector3 coasterPosition;
float coasterVelocity = COASTER_START_SPEED;

//Coaster Movement globals
float t = 1;
int currentTrackIndex = 0;
int currentSubSectionIndex = 0;
Vector3 startPos;
Vector3 endPos;

Vector3 getCoasterPosition()
{
	return coasterPosition;
}

void initRollerCoaster()
{
	up.x = 0;
	up.z = 0;
	up.y = 1;

	leftRail.topVerts = NULL;
	leftRail.bottomVerts = NULL;
	rightRail.topVerts = NULL;
	rightRail.bottomVerts = NULL;

	generateControlPoints();
}

static void generateControlPoints()
{
	numberOfControlPoints = 0;
	allocatedControlPoints = DEFAULT_NUMBER_OF_POINTS;
	controlPoints = calloc(allocatedControlPoints, sizeof(ControlPoint));

	defaultCoaster();

}

static void defaultCoaster()
{
	controlPoints[0].position.x = 0;
	controlPoints[0].position.y = 4;
	controlPoints[0].position.z = 10;
	controlPoints[0].isChain = 1;

	controlPoints[1].position.x = -12;
	controlPoints[1].position.y = 5;
	controlPoints[1].position.z = 10;
	controlPoints[1].isChain = 1;

	controlPoints[2].position.x = -16;
	controlPoints[2].position.y = 6;
	controlPoints[2].position.z = 2;
	controlPoints[2].isChain = 1;

	controlPoints[3].position.x = -16;
	controlPoints[3].position.y = 7;
	controlPoints[3].position.z = -4;
	controlPoints[3].isChain = 1;

	controlPoints[4].position.x = -16;
	controlPoints[4].position.y = 8;
	controlPoints[4].position.z = -12;
	controlPoints[4].isChain = 1;

	controlPoints[5].position.x = -10;
	controlPoints[5].position.y = 9;
	controlPoints[5].position.z = -18;
	controlPoints[5].isChain = 1;

	controlPoints[6].position.x = -6;
	controlPoints[6].position.y = 9.5;
	controlPoints[6].position.z = -18;

	controlPoints[6].position.x = 0;
	controlPoints[6].position.y = 9.5;
	controlPoints[6].position.z = -18;


	controlPoints[7].position.x = 10;
	controlPoints[7].position.y = 7.5;
	controlPoints[7].position.z = -18;

	controlPoints[8].position.x = 16;
	controlPoints[8].position.y = 7;
	controlPoints[8].position.z = -12;

	controlPoints[9].position.x = 10;
	controlPoints[9].position.y = 6;
	controlPoints[9].position.z = -6;

	controlPoints[10].position.x = 4;
	controlPoints[10].position.y = 7;
	controlPoints[10].position.z = -14;

	controlPoints[11].position.x = -10;
	controlPoints[11].position.y = 5.5;
	controlPoints[11].position.z = -12;

	controlPoints[12].position.x = -2;
	controlPoints[12].position.y = 5;
	controlPoints[12].position.z = -8;

	controlPoints[12].position.x = 4;
	controlPoints[12].position.y = 4.5;
	controlPoints[12].position.z = -4;

	controlPoints[13].position.x = 10;
	controlPoints[13].position.y = 4;
	controlPoints[13].position.z = 2;

	controlPoints[14].position.x = 10;
	controlPoints[14].position.y = 4;
	controlPoints[14].position.z = 10;

	numberOfControlPoints = 15;
}

void updateRollerCoaster()
{
	if(trackState == Constructing)
		takeInput();

	else if(trackState == Generating)
		generateTrack();

	else if (trackState == Ready)
	{
		moveCoaster();
		if(input[FinishTrack])
		{
			input[FinishTrack] = 0;
			trackState = Constructing;
		}
	}
}

static void generateTrack()
{
	if(trackSections == NULL)
		trackSections = calloc(numberOfControlPoints, sizeof(TrackSection));
	else
		trackSections = realloc(trackSections, sizeof(TrackSection) * numberOfControlPoints);

	Vector3 first;

	//=====TRACK SECTION GENERATION

	for(int k = 0; k < numberOfControlPoints; k++)
	{
		trackSections[k].isChain = controlPoints[k].isChain;

		int subSectionIndex = 0;
		for(float u = 0.0f; u < 1; u += (1.0 / NUMBER_OF_SUB_SECTIONS))
		{
			//Calculate new point
			Vector3 newPoint = qFunction(u, k);

			//Save first point for later
			if(k == 0 && subSectionIndex == 0)
				first = newPoint;

			//Assign as start of section
			trackSections[k].subSections[subSectionIndex].subSectionStart = newPoint;

			//Assign this point as the end of the previous section and calculate the length
			if(subSectionIndex - 1 >= 0){
				trackSections[k].subSections[subSectionIndex - 1].subSectionEnd = newPoint;
				calculateSubSectionLength(&(trackSections[k].subSections[subSectionIndex - 1]));
			}
			else if (k - 1 >= 0){
				trackSections[k - 1].subSections[NUMBER_OF_SUB_SECTIONS - 1].subSectionEnd = newPoint;
				calculateSubSectionLength(&(trackSections[k - 1].subSections[NUMBER_OF_SUB_SECTIONS - 1]));
			}

			subSectionIndex++;
		}
	}

	//Assign first point as the last point and calculate length
	trackSections[numberOfControlPoints - 1].subSections[NUMBER_OF_SUB_SECTIONS - 1].subSectionEnd = first;
	calculateSubSectionLength(&(trackSections[numberOfControlPoints - 1].subSections[NUMBER_OF_SUB_SECTIONS - 1]));

	//=====END TRACK SECTION GENERATION



	//=====RAIL VERTEX GENERATION
	float vertexsNeeded = numberOfControlPoints * NUMBER_OF_SUB_SECTIONS * 2;
	if(leftRail.topVerts == NULL)
		leftRail.topVerts = calloc(vertexsNeeded, sizeof(Vector3));
	else
		leftRail.topVerts = realloc(leftRail.topVerts, vertexsNeeded * sizeof(Vector3));

	if(leftRail.bottomVerts == NULL)
		leftRail.bottomVerts = calloc(vertexsNeeded, sizeof(Vector3));
	else
		leftRail.bottomVerts = realloc(leftRail.bottomVerts, vertexsNeeded * sizeof(Vector3));

	if(rightRail.topVerts == NULL)
		rightRail.topVerts = calloc(vertexsNeeded, sizeof(Vector3));
	else
		rightRail.topVerts = realloc(rightRail.topVerts, vertexsNeeded * sizeof(Vector3));

	if(rightRail.bottomVerts == NULL)
		rightRail.bottomVerts = calloc(vertexsNeeded, sizeof(Vector3));
	else
		rightRail.bottomVerts = realloc(rightRail.bottomVerts, vertexsNeeded * sizeof(Vector3));


	int vertIndex = 0;
	for(int i = 0; i < numberOfControlPoints; i++)
	{
		for(int j =0; j < NUMBER_OF_SUB_SECTIONS; j++)
		{
			
			//Calculate forward
			Vector3 currentPoint = trackSections[i].subSections[j].subSectionStart;
			Vector3 nextPoint = trackSections[i].subSections[j].subSectionEnd;
			Vector3 forward = minusVector3(&nextPoint, &currentPoint);

			Vector3 right = crossProductVector3(&forward, &up);
			right = NormalizeVector3(&right);
			right = multiplyVector3(&right, 0.25);
			

			Vector3 rightRailCenter;
			Vector3 leftRailCenter;


			rightRailCenter = addVector3(&currentPoint, &right);
			leftRailCenter = minusVector3(&currentPoint, &right);
			
			

			right = multiplyVector3(&right, 0.2);
			Vector3 left = multiplyVector3(&right, -1);


		
			leftRail.topVerts[vertIndex] = addVector3(&leftRailCenter, &left);
			rightRail.topVerts[vertIndex] = addVector3(&rightRailCenter, &left);

			vertIndex++;

			leftRail.topVerts[vertIndex] = addVector3(&leftRailCenter, &right);
			rightRail.topVerts[vertIndex] = addVector3(&rightRailCenter, &right);
			
			Vector3 down;
			down.x = 0;
			down.z = 0;
			down.y = -0.1f;

			leftRail.bottomVerts[vertIndex - 1] = addVector3(&(leftRail.topVerts[vertIndex - 1]), &down);
			rightRail.bottomVerts[vertIndex - 1] = addVector3(&(rightRail.topVerts[vertIndex - 1]), &down);

			leftRail.bottomVerts[vertIndex] = addVector3(&(leftRail.topVerts[vertIndex]), &down);
			rightRail.bottomVerts[vertIndex] = addVector3(&(rightRail.topVerts[vertIndex]), &down);

			vertIndex++;

		}

	}



	//=====END RAIL VERTEX GENERATION

	coasterPosition = trackSections[0].subSections[0].subSectionStart;
	trackState = Ready;

	t = 1;
	currentTrackIndex = 0;
	currentSubSectionIndex = 0;

	coasterVelocity = COASTER_START_SPEED;

	generateTrackDisplayList();
}

static void generateTrackDisplayList()
{
	trackList = glGenLists(1);

	glNewList(trackList, GL_COMPILE);

	drawFinishedTrack();

	glEndList();
}

/* Calculates the length of a subsection of track */
static void calculateSubSectionLength(TrackSubSection* subSection)
{
	Vector3 difference = minusVector3(&(subSection->subSectionEnd), &(subSection->subSectionStart));
	float length = magnitudeVector3(&difference);

	subSection->subSectionLength = length;
}


/* Moves the coaster and handles physics */
static void moveCoaster()
{
	if(input[Boost])
		coasterVelocity += BOOST_STRENGTH;

	if(trackSections[currentTrackIndex].isChain)
		if(coasterVelocity < CHAIN_LIFT_SPEED)
			coasterVelocity = CHAIN_LIFT_SPEED;


	float deltaT = (0.016 * coasterVelocity) / trackSections[currentTrackIndex].subSections[currentSubSectionIndex].subSectionLength;

	t += deltaT;

	while(t < 0 || t >= 1)
	{
		if (t >= 1)
		{
			//Select next track piece
			currentSubSectionIndex++;
			if (currentSubSectionIndex >= NUMBER_OF_SUB_SECTIONS)
			{
				currentSubSectionIndex = 0;
				currentTrackIndex++;

				if(currentTrackIndex >= numberOfControlPoints)
					currentTrackIndex = 0;
			}

			//Update lerp start position
			startPos = trackSections[currentTrackIndex].subSections[currentSubSectionIndex].subSectionStart;
			endPos = trackSections[currentTrackIndex].subSections[currentSubSectionIndex].subSectionEnd;

			//Update t and account for section switch in movement
			t -= 1;
			float percentOfTimeOnNextPiece = t / deltaT;

			deltaT = (0.016 * coasterVelocity) / trackSections[currentTrackIndex].subSections[currentSubSectionIndex].subSectionLength;
			t = deltaT * percentOfTimeOnNextPiece;
		}
		else if(t < 0)
		{
			//Select previous track piece
			currentSubSectionIndex--;
			if (currentSubSectionIndex < 0)
			{
				currentSubSectionIndex = NUMBER_OF_SUB_SECTIONS - 1;
				currentTrackIndex--;

				if(currentTrackIndex < 0)
					currentTrackIndex = numberOfControlPoints - 1;
			}

			//Update lerp positions
			startPos = trackSections[currentTrackIndex].subSections[currentSubSectionIndex].subSectionStart;
			endPos = trackSections[currentTrackIndex].subSections[currentSubSectionIndex].subSectionEnd;

			//Update t and account for section switch in movement
			t += 1;
			float percentOfTimeOnNextPiece = (1 - t) / deltaT;

			deltaT = (0.016 * coasterVelocity) / trackSections[currentTrackIndex].subSections[currentSubSectionIndex].subSectionLength;
			t = 1 - (deltaT * percentOfTimeOnNextPiece);
		}
	}



	//Move Coaster
	float u = (t / NUMBER_OF_SUB_SECTIONS) + currentSubSectionIndex * (1.0 / NUMBER_OF_SUB_SECTIONS);

	coasterPosition = qFunction(u, currentTrackIndex);
	

	

	//Physics
	//Determine the slope of the current track section and use it to apply gravity
	Vector3 subSectionVector = minusVector3(&endPos, &startPos);

	float hypotenuse = trackSections[currentTrackIndex].subSections[currentSubSectionIndex].subSectionLength;
	float opposite = subSectionVector.y;

	float angle = asin( opposite / hypotenuse);
	float slopeStrength = sin(angle);

	float deltaV = 0.016 * (GRAVITY * slopeStrength);
	coasterVelocity += deltaV;


	//A super simple friction model
	coasterVelocity = coasterVelocity * (1 - FRICTION_COEFFICIENT);
}

void drawRollerCoaster()
{
	if (trackState == Constructing)
		drawControlPoints();

	else if (trackState == Ready)
	{
		drawTrain();
		glCallList(trackList);
		//drawFinishedTrack();
	}
}


static void drawControlPoints()
{
	for(int i = 0; i < numberOfControlPoints; i++)
	{
		glPushMatrix();

		glTranslateVector3(&(controlPoints[i].position));

		if (selectedPoint == i)
			drawSquare(0.5);
		else
			drawSquare(0.22);

		glPopMatrix();
	}

	for(int i =0; i < numberOfControlPoints; i++)
	{
		glBegin(GL_LINES);

			glVertexVector3(&(controlPoints[i].position));

			if(i == numberOfControlPoints - 1)
				glVertexVector3(&(controlPoints[0].position));
			else
				glVertexVector3(&(controlPoints[i+1].position));

		glEnd();
	}

	
	glLineWidth(2);

	glBegin(GL_LINE_LOOP);

	for(int k = 0; k < numberOfControlPoints; k++)
	{
		if(controlPoints[k].isChain)
			glColor3f(1.0f, 1.0f, 0.0f);
		else
			glColor3f(1.0f, 0.0f, 1.0f);

		for(float u = 0.0f; u < 1; u += 0.25f)
		{
			Vector3 point = qFunction(u, k);
			glVertexVector3(&point);
		}	
	}

	glEnd();

}

static void drawTrain()
{
	glPushMatrix();
		glTranslateVector3(&coasterPosition);
		glColor3f(0.0f, 0.2f, 0.75f);
		glutSolidSphere(0.25, 5, 5);
	glPopMatrix();
}

static void drawFinishedTrack()
{
	glColor3f(1.0f, 0.0f, 1.0f);
	glLineWidth(2);


	// RAILS ================================================
	//TOP LEFT
	glBegin(GL_QUAD_STRIP);
	glColor3f(0.8f, 0.8f , 0.8f);
	int vertIndex = 0;
	for(int i = 0; i < numberOfControlPoints; i++)
	{
		for(int j =0; j < NUMBER_OF_SUB_SECTIONS; j++)
		{
			glVertexVector3(&(leftRail.topVerts[vertIndex]));
			vertIndex++;
			glVertexVector3(&(leftRail.topVerts[vertIndex]));
			vertIndex++;
		}
	}

	glVertexVector3(&(leftRail.topVerts[0]));
	glVertexVector3(&(leftRail.topVerts[1]));

	glEnd();


	//TOP RIGHT
	glBegin(GL_QUAD_STRIP);

	vertIndex = 0;
	for(int i = 0; i < numberOfControlPoints; i++)
	{
		for(int j =0; j < NUMBER_OF_SUB_SECTIONS; j++)
		{
			glVertexVector3(&(rightRail.topVerts[vertIndex]));
			vertIndex++;
			glVertexVector3(&(rightRail.topVerts[vertIndex]));
			vertIndex++;
		}
	}

	glVertexVector3(&(rightRail.topVerts[0]));
	glVertexVector3(&(rightRail.topVerts[1]));

	glEnd();



	//BOTTOM LEFT
	glBegin(GL_QUAD_STRIP);
	glColor3f(0.45f, 0.45f , 0.45f);
	vertIndex = 0;
	for(int i = 0; i < numberOfControlPoints; i++)
	{
		for(int j =0; j < NUMBER_OF_SUB_SECTIONS; j++)
		{
			glVertexVector3(&(leftRail.bottomVerts[vertIndex]));
			vertIndex++;
			glVertexVector3(&(leftRail.bottomVerts[vertIndex]));
			vertIndex++;
		}
	}

	glVertexVector3(&(leftRail.bottomVerts[0]));
	glVertexVector3(&(leftRail.bottomVerts[1]));

	glEnd();


	//BOTTOM RIGHT
	glBegin(GL_QUAD_STRIP);

	vertIndex = 0;
	for(int i = 0; i < numberOfControlPoints; i++)
	{
		for(int j =0; j < NUMBER_OF_SUB_SECTIONS; j++)
		{
			glVertexVector3(&(rightRail.bottomVerts[vertIndex]));
			vertIndex++;
			glVertexVector3(&(rightRail.bottomVerts[vertIndex]));
			vertIndex++;
		}
	}

	glVertexVector3(&(rightRail.bottomVerts[0]));
	glVertexVector3(&(rightRail.bottomVerts[1]));

	glEnd();




	//LEFT LEFT
	glBegin(GL_QUAD_STRIP);
	glColor3f(0.65f, 0.65f , 0.65f);
	vertIndex = 0;
	for(int i = 0; i < numberOfControlPoints; i++)
	{
		for(int j =0; j < NUMBER_OF_SUB_SECTIONS; j++)
		{
			glVertexVector3(&(leftRail.bottomVerts[vertIndex]));
			glVertexVector3(&(leftRail.topVerts[vertIndex]));
			vertIndex += 2;
		}
	}

	glVertexVector3(&(leftRail.bottomVerts[0]));
	glVertexVector3(&(leftRail.topVerts[0]));

	glEnd();


	//LEFT RIGHT
	glBegin(GL_QUAD_STRIP);
	glColor3f(0.65f, 0.65f , 0.65f);
	vertIndex = 0;
	for(int i = 0; i < numberOfControlPoints; i++)
	{
		for(int j =0; j < NUMBER_OF_SUB_SECTIONS; j++)
		{
			glVertexVector3(&(rightRail.bottomVerts[vertIndex]));
			glVertexVector3(&(rightRail.topVerts[vertIndex]));
			vertIndex += 2;
		}
	}

	glVertexVector3(&(rightRail.bottomVerts[0]));
	glVertexVector3(&(rightRail.topVerts[0]));

	glEnd();


	//RIGHT LEFT
	glBegin(GL_QUAD_STRIP);
	glColor3f(0.65f, 0.65f , 0.65f);
	vertIndex = 1;
	for(int i = 0; i < numberOfControlPoints; i++)
	{
		for(int j =0; j < NUMBER_OF_SUB_SECTIONS; j++)
		{
			glVertexVector3(&(leftRail.bottomVerts[vertIndex]));
			glVertexVector3(&(leftRail.topVerts[vertIndex]));
			vertIndex += 2;
		}
	}

	glVertexVector3(&(leftRail.bottomVerts[1]));
	glVertexVector3(&(leftRail.topVerts[1]));

	glEnd();


	//RIGHT RIGHT
	glBegin(GL_QUAD_STRIP);
	glColor3f(0.65f, 0.65f , 0.65f);
	vertIndex = 1;
	for(int i = 0; i < numberOfControlPoints; i++)
	{
		for(int j =0; j < NUMBER_OF_SUB_SECTIONS; j++)
		{
			glVertexVector3(&(rightRail.bottomVerts[vertIndex]));
			glVertexVector3(&(rightRail.topVerts[vertIndex]));
			vertIndex += 2;
		}
	}

	glVertexVector3(&(rightRail.bottomVerts[1]));
	glVertexVector3(&(rightRail.topVerts[1]));

	glEnd();
	

	// SUPPORTS ==========================
	
	glLineWidth(8);
	for(int i = 0; i < numberOfControlPoints; i++)
	{		
		glBegin(GL_LINES);

			//Main pillar
			Vector3 point = trackSections[i].subSections[0].subSectionStart;
			point.y -= 0.5;



			glVertexVector3(&point);
			point.y = -1;
			glVertexVector3(&point);
		

			glLineWidth(4);


			point = trackSections[i].subSections[0].subSectionStart;
			point.y -= 0.5;

			//Left rail connection
			glVertexVector3(&point);
			glVertexVector3(&(leftRail.topVerts[(i * NUMBER_OF_SUB_SECTIONS * 2)+1]));

			//Right rail connection
			glVertexVector3(&point);
			glVertexVector3(&(rightRail.topVerts[(i * NUMBER_OF_SUB_SECTIONS * 2)]));

		glEnd();		

	}


	// CHAIN LIFT ==================
	glLineWidth(1);
	glColor3f(0,0,0);
	for(int i=0; i<numberOfControlPoints; i++)
	{
		if(controlPoints[i].isChain)
		{
			glBegin(GL_LINE_STRIP);
			for(int j=0; j < NUMBER_OF_SUB_SECTIONS; j++)
			{
				Vector3 point = trackSections[i].subSections[j].subSectionStart;
				point.y -= 0.1;
				glVertexVector3(&point);	
			}
			glEnd();
		}
	}


}

//================INPUT FUNCTIONS=================

static void takeInput()
{
	if(input[FinishTrack]) {
		input[FinishTrack] = 0;
		trackState = Generating;
		return;
	}

	addPoint();
	removePoint();

	selectionInput();
	editControlPoint();
}

static void addPoint()
{
	if(input[Add] == 0 || selectedPoint == -1)
		return;
	input[Add] = 0;

	if(numberOfControlPoints + 1 > allocatedControlPoints)
		allocateMoreControlPoints();

	//Shift everything at and past the new point, up one index
	for(int i = numberOfControlPoints; i > selectedPoint; i--){
		controlPoints[i].position = controlPoints[i-1].position;
		controlPoints[i].isChain = controlPoints[i-1].isChain;
	}

	selectedPoint++;
	numberOfControlPoints++;
}

static void removePoint()
{
	if(input[Remove] == 0 || selectedPoint == -1 || numberOfControlPoints <= 3)
		return;
	input[Remove] = 0;

	//Shift all control points following the one to be removed, down one index
	for(int i = selectedPoint; i < numberOfControlPoints - 1; i++)
	{
			controlPoints[i].position = controlPoints[i+1].position;
			controlPoints[i].isChain = controlPoints[i+1].isChain;
	}

	numberOfControlPoints--;
}

static void allocateMoreControlPoints()
{
	allocatedControlPoints = allocatedControlPoints * 1.5;

	controlPoints = realloc(controlPoints, sizeof(Vector3) * allocatedControlPoints);

}

static void selectionInput()
{
	if (input[Next])
	{
		input[Next] = 0;
		selectedPoint++;
	}

	else if (input[Prev])
	{
		input[Prev] = 0;
		selectedPoint--;
	}
	else
		return;


	//Clamp selected point top valid numbers
	if (selectedPoint < 0)
		selectedPoint = numberOfControlPoints - 1;

	else if (selectedPoint >= numberOfControlPoints)
		selectedPoint = 0;
}

static void editControlPoint()
{
	if (selectedPoint == -1)
		return;

	//Toggle Chain
	if(input[ChainLift])
	{
		input[ChainLift] = 0;
		if(controlPoints[selectedPoint].isChain)
			controlPoints[selectedPoint].isChain = 0;
		else
			controlPoints[selectedPoint].isChain = 1;
	}
	//Adjust height
	if(input[Height])
	{
		controlPoints[selectedPoint].position.y += input[Height] * CONTROL_POINT_HEIGHT_STEP;
		input[Height] = 0;
	}
	if(input[Click] == 0)
		return;

	Transform* freeCamera = getFreeCameraTransform();

	//Remove the camera's x rotation
	float originalXRotation = freeCamera->rotation.x;
	freeCamera->rotation.x = 0;

	//Get the forward and right vectors relative to the camera
	Vector3 forward = TransformToForward(freeCamera);
	Vector3 right = TransformToRight(freeCamera);

	//Restore the camera's rotation
	freeCamera->rotation.x = originalXRotation;

	//Scale the vectors based on mouse movement
	forward = multiplyVector3(&forward, -input[MouseY] * CONTROL_POINT_MOVEMENT_SPEED);
	right = multiplyVector3(&right, input[MouseX] * CONTROL_POINT_MOVEMENT_SPEED);

	//Apply these vectors to the control point
	controlPoints[selectedPoint].position = addVector3(&(controlPoints[selectedPoint].position), &forward);
	controlPoints[selectedPoint].position = addVector3(&(controlPoints[selectedPoint].position), &right);

	

	consumeMouseInput();
}





/* Implementation of the q function provided in the lecture slides */
static Vector3 qFunction(float u, int i)
{
	float t = u;
	float sixth = (1.0 / 6.0);

	float tSquared = pow(t, 2);
	float tCubed   = pow(t, 3);


	float r0 = sixth * tCubed;
	float r1 = sixth * ( (-3 * tCubed) + (3 * tSquared) + (3 * t) + 1 );
	float r2 = sixth * ( (3 * tCubed) - (6 * tSquared) + 4 );
	float r3 = sixth * pow((1 - t), 3);

	Vector3 r3Vec;
	Vector3 r2Vec;
	Vector3 r1Vec;
	Vector3 r0Vec;

	if (i - 1 >= 0)
		r3Vec = multiplyVector3(&(controlPoints[i - 1].position), r3);
	else
		r3Vec = multiplyVector3(&(controlPoints[numberOfControlPoints + (i-1)].position), r3);

	r2Vec = multiplyVector3(&(controlPoints[i].position), r2);


	if (i + 1 < numberOfControlPoints)
		r1Vec = multiplyVector3(&(controlPoints[i + 1].position), r1);
	else
		r1Vec = multiplyVector3(&(controlPoints[0 + (i + 1 - numberOfControlPoints)].position), r1);

	if(i + 2 < numberOfControlPoints)
		r0Vec = multiplyVector3(&(controlPoints[i + 2].position), r0);
	else
		r0Vec = multiplyVector3(&(controlPoints[0 + (i + 2 - numberOfControlPoints)].position), r0);

	Vector3 finalVector;
	finalVector = addVector3(&r3Vec, &r2Vec);
	finalVector = addVector3(&finalVector, &r1Vec);
	finalVector = addVector3(&finalVector, &r0Vec);

	return finalVector;
}







