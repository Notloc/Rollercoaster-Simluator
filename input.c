/*  Input.c
 *  This module handles taking all input from the user, both keyboard and mouse
 */
#include "input.h"
#include <stdio.h>
#include <ctype.h>
#include <stdio.h>
#include <GL/glut.h>



//==============KEYBOARD======================

void initInput()
{
    memset(&input, 0, sizeof(input));
}

void keyPress(unsigned char key, int x, int y)
{
    key = tolower(key);
	switch (key)
	{
        case 'w':
            input[Up] = 1;
            break;
        case 'a':
            input[Left] = 1;
            break;
        case 's':
            input[Down] = 1;
            break;
        case 'd':
            input[Right] = 1;
            break;
        case 'q':
            input[FlyUp] = 1;
            break;
        case 'e':
            input[FlyDown] = 1;
            break;

		case 'p':
            if(input[Pause])
                input[Pause] = 0;
            else
                input[Pause] = 1;
			break;

        case '=':
            input[Add] = 1;
            break;

        case '-':
            input[Remove] = 1;
            break;

        case 'b':
            input[Boost] = 1;
            break;

        case 'c':
            input[ChainLift] = 1;
            break;

        case 'v':
            input[Camera] = 1;
            break;

        //Enter
        case 13:
            input[FinishTrack] = 1;
            break;
	}
}

void keyRelease(unsigned char key, int x, int y)
{
    key = tolower(key);
    switch(key)
    {
        case 'w':
            input[Up] = 0;
            break;
        case 'a':
            input[Left] = 0;
            break;
        case 's':
            input[Down] = 0;
            break;
        case 'd':
            input[Right] = 0;
            break;
        case 'q':
            input[FlyUp] = 0;
            break;
        case 'e':
            input[FlyDown] = 0;
            break;

        case 'b':
            input[Boost] = 0;
            break;
    }
}

void specialKeyPress(int key, int x, int y)
{
    switch (key)
    {
        //Right
        case 102:
            input[Next] = 1;
            break;

        //Left
        case 100:
            input[Prev] = 1;
            break;

        //Up
        case 101:
            input[Height] = 1;
            break;
        
        //Down
        case 103:
            input[Height] = -1;
            break;
    }
}

void specialKeyRelease(int key, int x, int y)
{
    //Nuthin'
}




//==============MOUSE=========================

/* Calculates and sets the mouse's position delta for the frame */
int prevX = 0, prevY = 0;
void mouseMovement(int x, int y)
{
    if(prevX == 0 && prevY == 0) {
        prevX = x;
        prevY = y;
        return;
    }

    input[MouseX] = x - prevX;
    input[MouseY] = y - prevY;

    prevX = x;
    prevY = y;
}

/* Flags the proper mouse button's state and resets tracked mouse movement on button release */
void mouseClick(int button, int state, int x, int y)
{
    if (button == 0)
    {
        if (state == GLUT_UP) {
            input[Click] = 0;
            prevY = 0;
            prevX = 0;
        }
        else
            input[Click] = 1;
    }

    else if (button == 2)
    {
        if (state == GLUT_UP) {
            input[AltClick] = 0;
            prevY = 0;
            prevX = 0;
        }
        else
            input[AltClick] = 1;
    }
}

/* Exists to prevent multiple actions from being performed with the same mouse movement */
void consumeMouseInput()
{
    input[MouseX] = 0;
    input[MouseY] = 0;
}