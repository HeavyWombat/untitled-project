#include<GL/gl.h>
#include<GL/glu.h>
#include<stdio.h>
#include<stdlib.h>

#include<display.h>
#include<landscape.h>

fake_image_t* heightMap;

float* heightMapPoints;

#define LANDSCAPE_FACTOR 6400.0 / heightMap->width
#define LANDSCAPE_MAX_HEIGHT 100.0

void landscape_init()
{
	// TODO: Create and fill the array for the hight map here
}

float landscape_calculateRed(float mapValue)
{
	return(0);
}

float landscape_calculateGreen(float mapValue)
{
	return(0);
}

float landscape_calculateBlue(float mapValue)
{
	return(0);
}

void landscape_draw()
{
	// TODO: Draw the interleaved arrays here
}
