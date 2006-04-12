#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <camera.h>
#include <landscape.h>
#include <gdv_resources.h>
#include <additionalMath.h>
#include <rain.h>

// --- ---

static const float g = 9.80665f;

static const float maxStartHeight = 100.0f;
static const float minStartHeight = 50.0f;

static const int rainDrops = 1000;
static rainDrop rain[ rainDrops ];

static GLuint rainDropList;

// --- ---

static void randomPosition( float* p )
{
  /*
  p[0] = cam.pos[0] + randomFloatNegPos() * 50.0f;
  p[1] = randomFloatPos() * ( maxStartHeight - minStartHeight ) + minStartHeight;
  p[2] = cam.pos[2] + randomFloatNegPos() * 50.0f;
  */
  
  p[0] = randomFloatPos() * ( LANDSCAPE_FACTOR * heightMap->width ) - X_OFFSET;
  p[1] = randomFloatPos() * ( maxStartHeight - minStartHeight ) + minStartHeight;
  p[2] = randomFloatNegPos() * ( 0.5f * LANDSCAPE_FACTOR * heightMap->height );
}

static void randomRainDrop( rainDrop& rainDrop )
{
  randomPosition( rainDrop.pos );
  rainDrop.dist = norm( rainDrop.pos );
  rainDrop.speed = 75.0f;
}

// --- ---

void rain_init( void )
{
  const float width = 0.4f;
  const float height = 2.0f * width;
  
  rainDropList = glGenLists( 1 );
  glNewList( rainDropList, GL_COMPILE );
  
  glBegin( GL_QUADS );
  
  glNormal3f( -1.0f, 0.0f, 0.0f );
  glTexCoord2i( 0, 0 );
  glVertex3f( -width, +height, 0.0f );
  
  glTexCoord2i( 0, 1 );
  glVertex3f( -width, -height, 0.0f );
  
  glTexCoord2i( 1, 1 );
  glVertex3f( +width, -height, 0.0f );
  
  glTexCoord2i( 1, 0 );
  glVertex3f( +width, +height, 0.0f );
  
  glEnd();
  
  glEndList();
  
  // ---
  
  for( int i=0; i<rainDrops; i++ )
    randomRainDrop( rain[i] );
}

void rain_draw( double timeDiff )
{
  qsort( rain, rainDrops, sizeof(rainDrop), rain_compareDist );
  
  glPushAttrib( GL_ENABLE_BIT );
  
  glDisable( GL_LIGHTING );
  
  glEnable( GL_TEXTURE_2D );
  glEnable( GL_BLEND );
  
  
  for( int i=0; i<rainDrops; i++ )
  {
    if( rain[i].dist > 4.0f )
    {
      glPushMatrix();
      
      glTranslatef( rain[i].pos[0], rain[i].pos[1], rain[i].pos[2] );
            
      float normal[3] = { 0.0f, 0.0f, 1.0f };
      float camera[3];
      float aRainDrop[3];
      float result[3];
      
      copyVec( cam.pos, camera );
      camera[1] = 0.0f;
    
      copyVec( rain[i].pos, aRainDrop );
      aRainDrop[1] = 0.0f;
      
      vecSub( camera, aRainDrop, result );
      float angle = calc2PIAngle( normal, result ) * M_1_PI * 180.0;
      
      glRotatef( angle, 0.0f, 1.0f, 0.0f );
      
      glBindTexture( GL_TEXTURE_2D, texture_rainDrop );
      glCallList( rainDropList );
      
      glPopMatrix();
      
      // ---
      
      rain[i].speed += g * timeDiff;
      rain[i].pos[1] -= ( 0.5 * g * timeDiff * timeDiff ) + ( rain[i].speed * timeDiff );
      
      if( rain[i].pos[1] < 0.0f )
        randomRainDrop( rain[i] );
        
      rain[i].dist = norm( rain[i].pos );
    }
  }
  
  glPopAttrib();
}

int rain_compareDist( const void* f0, const void* f1 )
{
  rainDrop fl0 = (rainDrop)( *((rainDrop*)f0) );
  rainDrop fl1 = (rainDrop)( *((rainDrop*)f1) );
  
  if( fl0.dist < fl1.dist )
    return 1;
  
  if( fl0.dist > fl1.dist )
    return -1;
  
  return 0;
}
