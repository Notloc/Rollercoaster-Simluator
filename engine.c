/*	Engine.c
 *	This module implements Vector3s and Transforms, as well as their associated functions
 *
 *	The setup bears some resemblance to that found in the Unity Engine, putting all positional data into one struct
 *	As well as providing functions that assist with 3D vectors 
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include "engine.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define RAD2DEG 180.0/M_PI

Transform* createTransform(Transform* parent)
{
	Transform* newTransform = malloc(sizeof(Transform));

	newTransform->parent = parent;

	newTransform->position.x = 0;
	newTransform->position.y = 0;
	newTransform->position.z = 0;

	newTransform->rotation.x = 0;
	newTransform->rotation.y = 0;
	newTransform->rotation.z = 0;

	return newTransform;
}

/*	Calculates and returns the Vector3 pointing right relative to the Transform */
Vector3 TransformToRight(Transform* transform)
{
	Vector3 direction;

	float rotationY = transform->rotation.y;
	float rotationZ = transform->rotation.z;

	direction.x = cos(rotationZ) * cos(rotationY);
	direction.y = sin(rotationZ);
	direction.z = cos(rotationZ) * -sin(rotationY);

	return direction;
}

/*	Calculates and returns the Vector3 pointing forward relative to the Transform */
Vector3 TransformToForward(Transform* transform)
{
	Vector3 direction;

	float rotationX = transform->rotation.x;
	float rotationY = transform->rotation.y + (M_PI / 2.0);

	direction.x = cos(rotationX) * cos(rotationY);
	direction.y = sin(rotationX);
	direction.z = cos(rotationX) * -sin(rotationY);

	return direction;
}

/*	Calculates and returns the Vector3 pointing up relative to the Transform */
Vector3 TransformToUp(Transform* transform)
{
	Vector3 direction;

	float rotationX = transform->rotation.x;
	float rotationZ = transform->rotation.z;

	direction.x = -sin(rotationZ);
	direction.y = cos(rotationX) * cos(rotationZ);
	direction.z = sin(rotationX) * cos(rotationZ);

	return direction;
}

Vector3 NormalizeVector3(Vector3* vector)
{
	float magnitude = magnitudeVector3(vector);

	return multiplyVector3(vector, 1.0 / magnitude);
}


Vector3* createVector3(float x, float y, float z)
{
	Vector3* newVector = malloc(sizeof(Vector3));

	newVector->x = x;
	newVector->y = y;
	newVector->z = z;

	return newVector;
}


Vector3 addVector3(Vector3* v1, Vector3* v2)
{
	Vector3 newVector;
	newVector.x = v1->x + v2->x;
	newVector.y = v1->y + v2->y;
	newVector.z = v1->z + v2->z;

	return newVector;
}

Vector3 minusVector3(Vector3* v1, Vector3* v2)
{
	Vector3 newVector;
	newVector.x = v1->x - v2->x;
	newVector.y = v1->y - v2->y;
	newVector.z = v1->z - v2->z;

	return newVector;
}

Vector3 multiplyVector3(Vector3* v1, float multiplier)
{
	Vector3 newVector;
	newVector.x = v1->x * multiplier;
	newVector.y = v1->y * multiplier;
	newVector.z = v1->z * multiplier;

	return newVector;
} 

Vector3 lerpVector3(Vector3* startPos, Vector3* endPos, float t)
{
	if(t < 0)
		t = 0;
	if(t > 1)
		t = 1;

	Vector3 difference = minusVector3(endPos, startPos);
	difference = multiplyVector3(&difference, t);

	return addVector3(startPos, &difference);
}

float magnitudeVector3(Vector3* vector)
{
	return sqrt(	pow(vector->x, 2) + pow(vector->y, 2) + pow(vector->z, 2)	);
}


Vector2 minusVector2(Vector2* v1, Vector2* v2)
{
	Vector2 newVector;
	newVector.x = v1->x - v2->x;
	newVector.y = v1->y - v2->y;

	return newVector;
}

float magnitudeVector2(Vector2* vector)
{
	return sqrt(	pow(vector->x, 2) + pow(vector->y, 2)	);
}



Vector3 crossProductVector3(Vector3* v1, Vector3* v2)
{
	Vector3 cross;

	cross.x = (v1->y * v2->z) - (v1->z * v2->y);
	cross.y = (v1->z * v2->x) - (v1->x * v2->z);
	cross.z = (v1->x * v2->y) - (v1->y * v2->x);

	return cross;
}



//========== Wrapper functions to allow calls using Vector3s

void glTranslateVector3(Vector3* vector)
{
	glTranslated(vector->x, vector->y, vector->z);
}

void glRotateVector3(Vector3* vector)
{
	glRotatef(vector->y * RAD2DEG, 0, 1, 0);
	glRotatef(vector->x * RAD2DEG, 1, 0, 0);
	glRotatef(vector->z * RAD2DEG, 0, 0, 1);
}

void glVertexVector3(Vector3* vector)
{
	glVertex3f(vector->x, vector->y, vector->z);
}

void lookAt(Vector3* eyes, Vector3* target, Vector3* up)
{
	gluLookAt(eyes->x, eyes->y,  eyes->z, target->x, target->y, target->z, up->x, up->y, up->z);
}

//==========



double myRandom(double min, double max)
{
	double	d;
	
	/* return a random number uniformly draw from [min,max] */
	d = min+(max-min)*(rand()%0x7fff)/32767.0;
	
	return d;
}