#ifndef __LANDSCAPE_H
#define __LANDSCAPE_H

#include<fakelib.h>

extern fake_image_t* heightMap;

float landscape_calculateRed(float mapValue);
float landscape_calculateGreen(float mapValue);
float landscape_calculateBlue(float mapValue);
void landscape_init();
void landscape_draw();
			   
#endif

