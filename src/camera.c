#include<math.h>

#include<camera.h>
#include<geometry.h>

camera cam;

void camera_init()
{
	cam.pos[0] = 40.0;
	cam.pos[1] = 10.0;
	cam.pos[2] = 40.0;

	cam.lookat[0] = 0.0;
	cam.lookat[1] = 10.0;
	cam.lookat[2] = 0.0;

	cam.upvec[0] = 0.0;
	cam.upvec[1] = 1.0;
	cam.upvec[2] = 0.0;
}

void camera_moveForward(float factor)
{
	float lookAtVector[3];

	camera_calcLookAtVector(lookAtVector);

	cam.pos[0] += lookAtVector[0] * factor;
	cam.pos[1] += lookAtVector[1] * factor;
	cam.pos[2] += lookAtVector[2] * factor;

	cam.lookat[0] += lookAtVector[0] * factor;
	cam.lookat[1] += lookAtVector[1] * factor;
	cam.lookat[2] += lookAtVector[2] * factor;
}

void camera_moveBackward(float factor)
{
	float lookAtVector[3];

	camera_calcLookAtVector(lookAtVector);

	cam.pos[0] -= lookAtVector[0] * factor;
	cam.pos[1] -= lookAtVector[1] * factor;
	cam.pos[2] -= lookAtVector[2] * factor;

	cam.lookat[0] -= lookAtVector[0] * factor;
	cam.lookat[1] -= lookAtVector[1] * factor;
	cam.lookat[2] -= lookAtVector[2] * factor;
}

void camera_moveUp(float factor)
{
	cam.pos[0] += cam.upvec[0] * factor;
	cam.pos[1] += cam.upvec[1] * factor;
	cam.pos[2] += cam.upvec[2] * factor;

	cam.lookat[0] += cam.upvec[0] * factor;
	cam.lookat[1] += cam.upvec[1] * factor;
	cam.lookat[2] += cam.upvec[2] * factor;
}

void camera_moveDown(float factor)
{
	cam.pos[0] -= cam.upvec[0] * factor;
	cam.pos[1] -= cam.upvec[1] * factor;
	cam.pos[2] -= cam.upvec[2] * factor;

	cam.lookat[0] -= cam.upvec[0] * factor;
	cam.lookat[1] -= cam.upvec[1] * factor;
	cam.lookat[2] -= cam.upvec[2] * factor;
}

void camera_strafeLeft(float factor)
{
	float sideVector[3];

	camera_calcSideVector(sideVector);

	cam.pos[0] -= sideVector[0] * factor;
	cam.pos[1] -= sideVector[1] * factor;
	cam.pos[2] -= sideVector[2] * factor;

	cam.lookat[0] -= sideVector[0] * factor;
	cam.lookat[1] -= sideVector[1] * factor;
	cam.lookat[2] -= sideVector[2] * factor;
}

void camera_strafeRight(float factor)
{
	float sideVector[3];

	camera_calcSideVector(sideVector);

	cam.pos[0] += sideVector[0] * factor;
	cam.pos[1] += sideVector[1] * factor;
	cam.pos[2] += sideVector[2] * factor;

	cam.lookat[0] += sideVector[0] * factor;
	cam.lookat[1] += sideVector[1] * factor;
	cam.lookat[2] += sideVector[2] * factor;
}

// rot um y achse
void camera_rotateYaw(float angle)
{
	float lookAtVector[4];
	float rotMat[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	float newLookAtVector[4];
	float length;

	lookAtVector[3] = 0.0;

	camera_calcLookAtVector(lookAtVector);
	geometry_getRotationMatrix(angle, cam.upvec[0], cam.upvec[1], cam.upvec[2], rotMat);
	geometry_multMatrixVector16x4(rotMat, lookAtVector, newLookAtVector);

	length = sqrt(newLookAtVector[0] * newLookAtVector[0] + newLookAtVector[1] * newLookAtVector[1] + newLookAtVector[2] * newLookAtVector[2]);

	newLookAtVector[0] /= length;
	newLookAtVector[1] /= length;
	newLookAtVector[2] /= length;

	cam.lookat[0] = cam.pos[0] + newLookAtVector[0];
	cam.lookat[1] = cam.pos[1] + newLookAtVector[1];
	cam.lookat[2] = cam.pos[2] + newLookAtVector[2];
}

// rot um x achse
void camera_rotatePitch(float angle)
{
	float lookAtVector[4];
	float sideVector[4];
	float upVector[4];
	float rotMat[16];
	float newLookAtVector[4];
	float newUpVector[4];
	float length;


	float rollAngle;
	float normalUp[3] = {0.0, 1.0, 0.0};


	lookAtVector[3] = 0.0;
	sideVector[3] = 0.0;
	upVector[0] = cam.upvec[0];
	upVector[1] = cam.upvec[1];
	upVector[2] = cam.upvec[2];
	upVector[3] = 0.0;
	newUpVector[3] = 0.0;

	camera_calcLookAtVector(lookAtVector);
	camera_calcSideVector(sideVector);
	geometry_getRotationMatrix(angle, sideVector[0], sideVector[1], sideVector[2], rotMat);
	geometry_multMatrixVector16x4(rotMat, lookAtVector, newLookAtVector);
	geometry_multMatrixVector16x4(rotMat, upVector, newUpVector);

	length = sqrt(newLookAtVector[0] * newLookAtVector[0] + newLookAtVector[1] * newLookAtVector[1] + newLookAtVector[2] * newLookAtVector[2]);

	newLookAtVector[0] /= length;
	newLookAtVector[1] /= length;
	newLookAtVector[2] /= length;

	length = sqrt(newUpVector[0] * newUpVector[0] + newUpVector[1] * newUpVector[1] + newUpVector[2] * newUpVector[2]);

	newUpVector[0] /= length;
	newUpVector[1] /= length;
	newUpVector[2] /= length;

	cam.lookat[0] = cam.pos[0] + newLookAtVector[0];
	cam.lookat[1] = cam.pos[1] + newLookAtVector[1];
	cam.lookat[2] = cam.pos[2] + newLookAtVector[2];

	cam.upvec[0] = newUpVector[0];
	cam.upvec[1] = newUpVector[1];
	cam.upvec[2] = newUpVector[2];

	
	rollAngle = asin((sideVector[1] * normalUp[1])/(sqrt(sideVector[0] * sideVector[0] + sideVector[1] * sideVector[1] + sideVector[2] * sideVector[2])));
	camera_rotateRoll(rollAngle);
}

void camera_rotateRoll(float angle)
{
	float lookAtVector[4];
	float rotMat[16];
	float upVector[4];
	float newUpVector[4];
	float length;

	lookAtVector[3] = 0.0;
	upVector[0] = cam.upvec[0];
	upVector[1] = cam.upvec[1];
	upVector[2] = cam.upvec[2];
	upVector[3] = 0.0;
	newUpVector[3] = 0.0;

	camera_calcLookAtVector(lookAtVector);
	geometry_getRotationMatrix(angle, lookAtVector[0], lookAtVector[1], lookAtVector[2], rotMat);
	geometry_multMatrixVector16x4(rotMat, upVector, newUpVector);

	length = sqrt(newUpVector[0] * newUpVector[0] + newUpVector[1] * newUpVector[1] + newUpVector[2] * newUpVector[2]);

	newUpVector[0] /= length;
	newUpVector[1] /= length;
	newUpVector[2] /= length;

	cam.upvec[0] = newUpVector[0];
	cam.upvec[1] = newUpVector[1];
	cam.upvec[2] = newUpVector[2];
}


// private

void camera_calcLookAtVector(float retLookAtVector[3])
{
	float length;

	//geometry_vector_sub_3f();
	retLookAtVector[0] = cam.lookat[0] - cam.pos[0];
	retLookAtVector[1] = cam.lookat[1] - cam.pos[1];
	retLookAtVector[2] = cam.lookat[2] - cam.pos[2];

	length = sqrt(retLookAtVector[0] * retLookAtVector[0] + retLookAtVector[1] * retLookAtVector[1] + retLookAtVector[2] * retLookAtVector[2]);

	retLookAtVector[0] /= length;
	retLookAtVector[1] /= length;
	retLookAtVector[2] /= length;
}

void camera_calcSideVector(float retSideVector[3])
{
	float lookAtVector[3];
	float length;

	camera_calcLookAtVector(lookAtVector);

	retSideVector[0] = lookAtVector[1] * cam.upvec[2] -  lookAtVector[2] * cam.upvec[1];
	retSideVector[1] = lookAtVector[2] * cam.upvec[0] -  lookAtVector[0] * cam.upvec[2];
	retSideVector[2] = lookAtVector[0] * cam.upvec[1] -  lookAtVector[1] * cam.upvec[0];

	length = sqrt(retSideVector[0] * retSideVector[0] + retSideVector[1] * retSideVector[1] + retSideVector[2] * retSideVector[2]);

	retSideVector[0] /=length;
	retSideVector[1] /=length;
	retSideVector[2] /=length;
}
