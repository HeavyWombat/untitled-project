#ifndef __LANDSCAPE_H
#define __LANDSCAPE_H

#include <fakelib.h>

#define LANDSCAPE_FACTOR       ( 256.0 / heightMap->width )
#define LANDSCAPE_MAX_HEIGHT   10.0f
#define X_OFFSET               25.0f
#define Z_OFFSET               ( LANDSCAPE_FACTOR * heightMap->height * 0.5f )

typedef struct
{
  int mark;
  
  float n0[3];
  float p0[3];

  float n1[3];
  float p1[3];

  float n2[3];
  float p2[3];
  
  float n[3];
  
} faceWithNormal;

extern fake_image_t* heightMap;

void landscape_init();
void landscape_draw();
float getHeightAtPos( float x, float z );
			   
#endif

