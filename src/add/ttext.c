#include <stdio.h>
#include <string.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <gdv_resources.h>
#include <ttext.h>

// --- ---

static float height = 0.075f;
static float width = height / sqrt( 2.0f );
static float narrowValue = height * 0.3f;
static GLuint ttext[256];

// --- ---

// --- ---

void ttext_setHeight( float h )
{
  height = h;
  width = height / sqrt( 2.0f );
  narrowValue = height * 0.3f;
  
  glDeleteLists( ttext[0], 256 );
  ttext_init();
}

void ttext_init( void )
{
  int idx = 0;
  
  ttext[0] = glGenLists( 256 );
  
  float yP = 0.0f;
  float zP = 0.0f;

  float leftOffset = 0.0f;
  float topOffset = 0.0f;
  float cWidth = 32.0f / 512.0f;
  float cHeight = 32.0f / 512.0f;

  for( int i=0; i<16; i++ )
  {
    for( int j=0; j<16; j++ )
    {
      leftOffset = (float)j * cWidth;
      topOffset = (float)i * cHeight;
      
      ttext[idx] = ttext[0] + idx;
      
      glNewList( ttext[idx], GL_COMPILE );
      
      glBegin( GL_QUADS );
      glTexCoord2f( leftOffset, topOffset + cHeight );
      glVertex2f( yP, zP );
      
      glTexCoord2f( leftOffset + cWidth, topOffset + cHeight );
      glVertex2f( yP + width, zP );
      
      glTexCoord2f( leftOffset + cWidth, topOffset );
      glVertex2f( yP + width, zP + height );
      
      glTexCoord2f( leftOffset, topOffset );
      glVertex2f( yP, zP + height );
      glEnd();
      
      glTranslatef( width - narrowValue, 0.0f, 0.0f );
      
      glEndList();
  
      idx++;
    }
  }  
}

void ttext_drawText( float y, float z, char* text )
{
  y = ( y > 1.0f )? 1.0f - ( strlen( text ) + 1 ) * ( width - narrowValue ): 2.0f * y - 1.0f;
  z = ( z > 1.0f )? -1.0f: ( -2.0f * z + 1.0f) - height;
    
  glPushAttrib( GL_ENABLE_BIT );
  
  glDisable( GL_DEPTH_TEST );
  glDisable( GL_LIGHTING );
  
  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();
  
  // ---

  glTranslatef( y, z, 0.0f );

  glEnable( GL_TEXTURE_2D );
  glEnable( GL_BLEND );
  
  glBindTexture( GL_TEXTURE_2D, texture_text );
      
  glListBase( ttext[0] );
  glCallLists( strlen(text), GL_UNSIGNED_BYTE, text );
  
  // ---    

  glPopMatrix();
  glPopMatrix();
  glPopAttrib();
}
