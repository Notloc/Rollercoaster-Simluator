/*  RollerCoaster Simulator
 *  Colton Campbell
 *  B00693513
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <GL/glut.h>

#include "main.h"
#include "engine.h" 
#include "camera.h"
#include "input.h"
#include "rollercoaster.h"


#define FRAME_TIME 0.016
#define FRAME_TIME_MS 16

#define FOV 85


static void init(void);
static void onUpdate(int value);
static void onDisplay(void);
static void onReshape(int w, int h);
static void drawWorld();

int paused = 0;

int main(int argc, char *argv[])
{
    srand((unsigned int) time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    
    //Window
    glutInitWindowSize(500, 500);
    glutCreateWindow("Rollercoaster");
	glutReshapeFunc(onReshape);
    glutDisplayFunc(onDisplay);

	glClearColor(0.2, 0.9, 1, 1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);

    //Keyboard
    glutIgnoreKeyRepeat(1);
    glutKeyboardFunc(keyPress);
    glutKeyboardUpFunc(keyRelease);
    glutSpecialFunc(specialKeyPress);
    glutSpecialUpFunc(specialKeyRelease);
    
    
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseMovement);


    glutTimerFunc(FRAME_TIME_MS, onUpdate, 0);

    init();
    glutMainLoop();

    return 0;
}

static void init()
{
    initInput();
	initCamera();
	initRollerCoaster();
}

static void onUpdate(int value)
{
    if(input[Pause])
    {
        input[Pause] = 0;
        if(paused)
            paused = 0;
        else
            paused = 1;
    }

    glutTimerFunc(16, onUpdate, value);

    //Return now if paused
    if(paused)
        return;

    //Update state of program
	updateCamera();
	updateRollerCoaster();

	//Render frame
    glutPostRedisplay();
}


static void onDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();


    applyCamera();
    drawWorld();
    drawRollerCoaster();
    

    glutSwapBuffers();
}


static void onReshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect = w/(float)h;

    gluPerspective(FOV, aspect, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
}


static void drawWorld()
{
	glColor3f(0.3f, 0.9f, 0.3f);

	float groundSize = 500.0f;
	glBegin(GL_POLYGON);
		glVertex3f(-0.5f * groundSize, 0, 0.5f * groundSize);
		glVertex3f(0.5f * groundSize, 0, 0.5f * groundSize);
		glVertex3f(0.5f * groundSize, 0, -0.5f * groundSize);
		glVertex3f(-0.5f * groundSize, 0, -0.5f * groundSize);
	glEnd();
}