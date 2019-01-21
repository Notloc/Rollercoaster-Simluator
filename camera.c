/*	Camera.c
 *	This module controls the camera
 *	There are 3 modes for the camera:
 *		OrbitingCamera	-	The camera rotates around the Roller Coaster
 *		CoasterCamera	-	The camera rides the Roller Coaster, you can look around with right mouse
 *		FreeCamera 		-	The camera can be freely controlled using WASD and right mouse  
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "engine.h"
#include "camera.h"
#include "input.h"
#include "rollercoaster.h"

#define CAMERA_SPEED 0.15
#define CAMERA_ROTATION_SPEED 2.5

#define ORBIT_HEIGHT 20
#define ORBIT_SPEED 0.01
#define ORBIT_RADIUS 15

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static void updateOrbitCamera(void);


Vector3* Vector3Up;
Vector3* cameraOrbitPosition;
Vector3* cameraOrbitTarget;

Transform* freeCameraTransform;
Transform* coasterCamTransform;

typedef enum {OrbitingCamera, CoasterCamera, FreeCamera} CameraMode;
CameraMode cameraMode = FreeCamera;

Transform* getFreeCameraTransform()
{
	return freeCameraTransform;
}

void initCamera()
{
	freeCameraTransform = createTransform(NULL);
	coasterCamTransform = createTransform(NULL);

	Vector3 position = {0, 10, 4};
	freeCameraTransform->position = position;

	Vector3Up = createVector3(0,1,0);

	cameraOrbitPosition = createVector3(0, ORBIT_HEIGHT, 0);
	cameraOrbitTarget = createVector3(0,0,0);
}

void updateCamera()
{
	if(input[Camera])
	{
		input[Camera] = 0;

		cameraMode++;
		if(cameraMode >= 3)
			cameraMode = 0;

	}

	if(cameraMode == OrbitingCamera)
	{
		updateOrbitCamera();
	}
	else if(cameraMode == FreeCamera)
	{
		if(input[Left])
		{
			Vector3 myRight = TransformToRight(freeCameraTransform);
			myRight = multiplyVector3(&myRight, CAMERA_SPEED);

			freeCameraTransform->position = minusVector3( &(freeCameraTransform->position), &myRight );
		}
		else if(input[Right])
		{
			Vector3 myRight = TransformToRight(freeCameraTransform);
			myRight = multiplyVector3(&myRight, CAMERA_SPEED);

			freeCameraTransform->position = addVector3( &myRight, &(freeCameraTransform->position));
		}

		if(input[Up])
		{
			Vector3 myForward = TransformToForward(freeCameraTransform);
			myForward = multiplyVector3(&myForward, CAMERA_SPEED);

			freeCameraTransform->position = addVector3( &myForward, &(freeCameraTransform->position));
		}
		else if(input[Down])
		{
			Vector3 myForward = TransformToForward(freeCameraTransform);
			myForward = multiplyVector3(&myForward, CAMERA_SPEED);

			freeCameraTransform->position = minusVector3(&(freeCameraTransform->position), &myForward);
		}

		if(input[FlyUp])
		{
			Vector3 myUp;
			myUp.z = 0;
			myUp.x = 0;
			myUp.y = 1;

			myUp = multiplyVector3(&myUp, CAMERA_SPEED);

			freeCameraTransform->position = addVector3(&(freeCameraTransform->position), &myUp);
		}
		else if(input[FlyDown])
		{
			Vector3 myUp;
			myUp.z = 0;
			myUp.x = 0;
			myUp.y = 1;

			myUp = multiplyVector3(&myUp, CAMERA_SPEED);

			freeCameraTransform->position = minusVector3(&(freeCameraTransform->position), &myUp);
		}

		if(input[AltClick]) {
			rotateCamera(freeCameraTransform, input[MouseX], input[MouseY]);
			consumeMouseInput();
		}
	}
	else if(cameraMode == CoasterCamera)
	{
		if(input[AltClick]) {
			rotateCamera(coasterCamTransform, input[MouseX], input[MouseY]);
			consumeMouseInput();
		}
	}
}

void rotateCamera(Transform* transform, int x, int y)
{
    transform->rotation.x -= (y / 180.0) * CAMERA_ROTATION_SPEED;
    transform->rotation.y -= (x / 180.0) * CAMERA_ROTATION_SPEED;

    if(transform->rotation.x >= (M_PI / 2.0))
    	transform->rotation.x = (M_PI / 2.0) - 0.01;

    if(transform->rotation.x <= (-M_PI / 2.0))
    	transform->rotation.x = (-M_PI / 2.0) + 0.01;

}

float orbitTheta = M_PI * (0.5);
static void updateOrbitCamera()
{
	cameraOrbitPosition->x = cos(orbitTheta) * ORBIT_RADIUS;
	cameraOrbitPosition->z = sin(orbitTheta) * ORBIT_RADIUS;

	orbitTheta += ORBIT_SPEED;
}

void applyCamera()
{
	if(cameraMode == OrbitingCamera)
	{
		lookAt(cameraOrbitPosition, cameraOrbitTarget, Vector3Up);
	}
	else if(cameraMode == FreeCamera)
	{
		Vector3 myForward = TransformToForward(freeCameraTransform);
		myForward = addVector3(&(freeCameraTransform->position), &myForward);

		lookAt(&(freeCameraTransform->position), &myForward, Vector3Up);
	}
	else if(cameraMode == CoasterCamera)
	{
		coasterCamTransform->position = getCoasterPosition();
		coasterCamTransform->position.y += 1;

		Vector3 myForward = TransformToForward(coasterCamTransform);
		myForward = addVector3(&(coasterCamTransform->position), &myForward);

		lookAt(&(coasterCamTransform->position), &myForward, Vector3Up);
	}
}