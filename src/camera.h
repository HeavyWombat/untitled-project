#ifndef __CAMERA_H
#define __CAMERA_H

typedef struct struct_camera
{
	float pos[3];
	float lookat[3];
	float upvec[3];
} camera;

extern camera cam;

void camera_init();
void camera_moveForward(float factor);
void camera_moveBackward(float factor);
void camera_moveUp(float factor);
void camera_moveDown(float factor);
void camera_strafeLeft(float factor);
void camera_strafeRight(float factor);
void camera_rotateYaw(float angle);
void camera_rotatePitch(float angle);
void camera_rotateRoll(float angle);
void camera_calcLookAtVector(float retLookAtVector[3]);
void camera_calcSideVector(float retSideVector[3]);

#endif
