#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <light.h>
#include <camera.h>
#include <gdv_resources.h>
#include <landscape.h>
#include <sound.h>
#include <additionalMath.h>
#include <environment.h>

// --- ---

static GLuint lightningList;

// --- ---

// --- ---

void environment_init( void )
{
  const float heightLightning = 250.0f;
  const float widthLightning = 0.25f * heightLightning;
  
  lightningList = glGenLists( 1 );
  glNewList( lightningList, GL_COMPILE );
  
  glBegin( GL_QUADS );
  
  glNormal3f( 0.0f, 0.0f, 1.0f );
  glTexCoord2i( 0, 0 );
  glVertex3f( -widthLightning, heightLightning, 0.0f );
  
  glTexCoord2i( 0, 1 );
  glVertex3f( -widthLightning, 0.0f, 0.0f );
  
  glTexCoord2i( 1, 1 );
  glVertex3f( +widthLightning, 0.0f, 0.0f );
  
  glTexCoord2i( 1, 0 );
  glVertex3f( +widthLightning, heightLightning, 0.0f );
  
  glEnd();
  
  glEndList();
}

void environment_draw( double timeDiff )
{
  static int thunder = 0;
  static double thunderLengthCounter = 0.0;
  if( thunder == 1 )
    thunderLengthCounter += timeDiff;
   
  static double thunderCounter = 0.0;
  static double nextThunder = 20.0;
  thunderCounter += timeDiff;
  
  static float fullAngle = 0.0f;
  
  // ---
  
  if( thunderCounter > nextThunder )
  {
    fullAngle = 180.0f * randomFloatNegPos();

    sound_thunderSound();
    
    light_thunder( 1 );
    thunder = 1;
    
    nextThunder = 30.0 * randomFloatPos();
    thunderCounter = 0.0;
  }
  
  if( thunder == 1 )
  {
    float currentPos[3] = { 300.0f, 0.0f, 0.0f };
    
    float normal[3] = { 0.0f, 0.0f, 1.0f };
    float camera[3];
    float lightning[3];
    float result[3];
    
    copyVec( cam.pos, camera );
    camera[1] = 0.0f;
  
    copyVec( currentPos, lightning );
    lightning[1] = 0.0f;
    
    vecSub( camera, lightning, result );
    float angle = calc2PIAngle( normal, result ) * M_1_PI * 180.0;
  
    glPushAttrib( GL_ENABLE_BIT );
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_BLEND );
    
    glPushMatrix();
    
    glRotatef( fullAngle, 0.0f, 1.0f, 0.0f );
    glTranslatef( currentPos[0], currentPos[1], currentPos[2] );
    glRotatef( angle, 0.0f, 1.0f, 0.0f );
    
    glBindTexture( GL_TEXTURE_2D, texture_lightning );
    glCallList( lightningList );
    
    glPopMatrix();
    
    glPopAttrib();
    
    // ---

    if( thunderLengthCounter > 0.1f )
    {
      thunderLengthCounter = 0.0;
      thunder = 0;
      
      light_thunder( 0 );
    }
  }
}
