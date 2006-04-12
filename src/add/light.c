#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <math.h>

#include <light.h>

// --- ---

static GLfloat lightAmbient0[]  = {  0.6f,  0.6f,  0.6f,  1.0f };
static GLfloat lightDiffuse0[]  = {  0.8f,  0.8f,  0.8f,  1.0f };
static GLfloat lightSpecular0[] = {  1.0f,  1.0f,  1.0f,  1.0f };
static GLfloat lightPosition0[] = { -1.0f,  1.0f, -1.0f,  0.0f };

static GLfloat lightAmbient1[]  = {    0.7f,    0.7f,    0.7f,  1.0f };
static GLfloat lightDiffuse1[]  = {    0.7f,    0.7f,    0.7f,  1.0f };
static GLfloat lightSpecular1[] = {    1.0f,    1.0f,    1.0f,  1.0f };
static GLfloat lightPosition1[] = { -100.0f,  100.0f, -100.0f,  1.0f };

static GLfloat lightAmbient2[]  = {    1.0f,    1.0f,    1.0f,  1.0f };
static GLfloat lightDiffuse2[]  = {    1.0f,    1.0f,    1.0f,  1.0f };
static GLfloat lightSpecular2[] = {    1.0f,    1.0f,    1.0f,  1.0f };
static GLfloat lightPosition2[] = { -100.0f,  100.0f, -100.0f,  1.0f };

// --- ---

// --- ---

void light_init( void )
{
  GLfloat fogColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };

  glFogi( GL_FOG_MODE, GL_EXP );
  glFogfv( GL_FOG_COLOR, fogColor );
  glFogf( GL_FOG_DENSITY, 0.004f );

  glEnable( GL_LIGHTING );
	
  glEnable( GL_LIGHT1 );
  
  glDisable( GL_LIGHT2 );
  
  glDisable( GL_LIGHT0 );
  glEnable( GL_FOG );
}

void light_setup( void )
{
  glLightfv( GL_LIGHT0, GL_DIFFUSE, lightDiffuse0 );
  glLightfv( GL_LIGHT0, GL_AMBIENT, lightAmbient0 );
  glLightfv( GL_LIGHT0, GL_SPECULAR, lightSpecular0 );
  glLightfv( GL_LIGHT0, GL_POSITION, lightPosition0 );
  
  glLightfv( GL_LIGHT1, GL_DIFFUSE, lightDiffuse1 );
  glLightfv( GL_LIGHT1, GL_AMBIENT, lightAmbient1 );
  glLightfv( GL_LIGHT1, GL_SPECULAR, lightSpecular1 );
  glLightfv( GL_LIGHT1, GL_POSITION, lightPosition1 );
  
  glLightfv( GL_LIGHT2, GL_DIFFUSE, lightDiffuse2 );
  glLightfv( GL_LIGHT2, GL_AMBIENT, lightAmbient2 );
  glLightfv( GL_LIGHT2, GL_SPECULAR, lightSpecular2 );
  glLightfv( GL_LIGHT2, GL_POSITION, lightPosition2 );
}

void light_thunder( int status )
{
  if( status == 1 )
  {
    glEnable( GL_LIGHT2 );
  }
  else
  {
    glDisable( GL_LIGHT2 );
  }
}
