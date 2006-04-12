#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <skybox.h>
#include <gdv_resources.h>
#include <display.h>

static int skyBoxList;

void skybox_init( void )
{
  float halfSideLength = 512.0f;
  float halfSideHeight = 0.5f * halfSideLength;
  
  skyBoxList = glGenLists( 1 );
  glNewList( skyBoxList, GL_COMPILE );
  
  glPushAttrib( GL_ENABLE_BIT );

  glDisable( GL_DEPTH_TEST );
  glDisable( GL_LIGHTING );
  
  glEnable( GL_TEXTURE_2D );
  
  glPushMatrix();

  // glTranslatef( 0.0f, 0.5f * halfSideHeight, 0.0f );
  
  glBindTexture( GL_TEXTURE_2D, texture_skybox_top );
  glBegin( GL_QUADS );
  glTexCoord2f( 0.0f, 0.0f );
  glVertex3f( -1.0f * halfSideLength,         halfSideHeight,         halfSideLength );
  glTexCoord2f( 0.0f, 1.0f );
  glVertex3f( -1.0f * halfSideLength,         halfSideHeight, -1.0f * halfSideLength );
  glTexCoord2f( 1.0f, 1.0f );
  glVertex3f(         halfSideLength,         halfSideHeight, -1.0f * halfSideLength );
  glTexCoord2f( 1.0f, 0.0f );
  glVertex3f(         halfSideLength,         halfSideHeight,         halfSideLength );
  glEnd();

  glBindTexture( GL_TEXTURE_2D, texture_skybox_left );
  glBegin( GL_QUADS );
  glTexCoord2f( 0.0f, 1.0f );
  glVertex3f( -1.0f * halfSideLength, -1.0f * halfSideHeight,         halfSideLength );
  glTexCoord2f( 1.0f, 1.0f );
  glVertex3f( -1.0f * halfSideLength, -1.0f * halfSideHeight, -1.0f * halfSideLength );
  glTexCoord2f( 1.0f, 0.0f );
  glVertex3f( -1.0f * halfSideLength,         halfSideHeight, -1.0f * halfSideLength );
  glTexCoord2f( 0.0f, 0.0f );
  glVertex3f( -1.0f * halfSideLength,         halfSideHeight,         halfSideLength );
  glEnd();

  glBindTexture( GL_TEXTURE_2D, texture_skybox_right );
  glBegin( GL_QUADS );
  glTexCoord2f( 0.0f, 1.0f );
  glVertex3f(         halfSideLength, -1.0f * halfSideHeight, -1.0f * halfSideLength );
  glTexCoord2f( 1.0f, 1.0f );
  glVertex3f(         halfSideLength, -1.0f * halfSideHeight,         halfSideLength );
  glTexCoord2f( 1.0f, 0.0f );
  glVertex3f(         halfSideLength,         halfSideHeight,         halfSideLength );
  glTexCoord2f( 0.0f, 0.0f );
  glVertex3f(         halfSideLength,         halfSideHeight, -1.0f * halfSideLength );
  glEnd();

  glBindTexture( GL_TEXTURE_2D, texture_skybox_front );
  glBegin( GL_QUADS );
  glTexCoord2f( 0.0f, 1.0f );
  glVertex3f(         halfSideLength, -1.0f * halfSideHeight,         halfSideLength );
  glTexCoord2f( 1.0f, 1.0f );
  glVertex3f( -1.0f * halfSideLength, -1.0f * halfSideHeight,         halfSideLength );
  glTexCoord2f( 1.0f, 0.0f );
  glVertex3f( -1.0f * halfSideLength,         halfSideHeight,         halfSideLength );
  glTexCoord2f( 0.0f, 0.0f );
  glVertex3f(         halfSideLength,         halfSideHeight,         halfSideLength );
  glEnd();

  glBindTexture( GL_TEXTURE_2D, texture_skybox_back );
  glBegin( GL_QUADS );
  glTexCoord2f( 0.0f, 1.0f );
  glVertex3f( -1.0f * halfSideLength, -1.0f * halfSideHeight, -1.0f * halfSideLength );
  glTexCoord2f( 1.0f, 1.0f );
  glVertex3f(         halfSideLength, -1.0f * halfSideHeight, -1.0f * halfSideLength );
  glTexCoord2f( 1.0f, 0.0f );
  glVertex3f(         halfSideLength,         halfSideHeight, -1.0f * halfSideLength );
  glTexCoord2f( 0.0f, 0.0f );
  glVertex3f( -1.0f * halfSideLength,         halfSideHeight, -1.0f * halfSideLength );
  glEnd();

  glBindTexture( GL_TEXTURE_2D, texture_skybox_bottom );
  glBegin( GL_QUADS );
  glTexCoord2f( 1.0f, 1.0f );
  glVertex3f(         halfSideLength, -1.0f * halfSideHeight,         halfSideLength );
  glTexCoord2f( 1.0f, 0.0f );
  glVertex3f(         halfSideLength, -1.0f * halfSideHeight, -1.0f * halfSideLength );
  glTexCoord2f( 0.0f, 0.0f );
  glVertex3f( -1.0f * halfSideLength, -1.0f * halfSideHeight, -1.0f * halfSideLength );
  glTexCoord2f( 0.0f, 1.0f );
  glVertex3f( -1.0f * halfSideLength, -1.0f * halfSideHeight,         halfSideLength );
  glEnd();

  glPopMatrix();

  glPopAttrib();
  
  glEndList();
}

void skybox_draw( void )
{
  glCallList( skyBoxList );
}
