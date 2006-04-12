#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <gdv_os.h>
#include <display.h>
#include <camera.h>
#include <gdv_resources.h>
#include <light.h>
#include <additionalMath.h>
#include <cameraMovement.h>
#include <landscape.h>
#include <skybox.h>
#include <trees.h>
#include <objects.h>
#include <rifle.h>
#include <rain.h>
#include <environment.h>
#include <targets.h>

#define FPS_BUFFER_SIZE 10000

// --- ---

int display_width;
int display_height;

int display_showEmpty = 0;
int display_showFPS = 0;

float fpsBuffer[FPS_BUFFER_SIZE] = {0};
int fpsBufferCursor = 0;

float aspectRatio = (float)display_width/(float)display_height;
float minDist = 0.1f;
float maxDist = 32000.0f;

// --- ---

// --- ---

void display_init()
{
  glClearAccum( 0.0, 0.0, 0.0, 1.0 );
  glClear( GL_ACCUM_BUFFER_BIT );
}

void display_setSize( int w, int h )
{
	display_width = w;
	display_height = h;
  aspectRatio = (float)display_width / (float)display_height;
}

void display_drawScene( double timeDiff )
{
  static int i = 1;
  static float deltas[3] = { -1.0f, 0.0f, 1.0f };
  
  if( bluring == 1 )
  {
    i = ( i++ ) % 3;
  }
  else
  {
    i = 1;
  }
  
	glClearColor( 0.0f, 0.0f, 0.2f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glViewport( 0, 0, display_width, display_height );

	glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  switch( cameraMode )
  {
    case CAMERA_MODE_WALK:
    case CAMERA_MODE_FREE:
      gluPerspective( defaultFOV, aspectRatio, minDist, maxDist );
      break;
    
    case CAMERA_MODE_OPT:
      gluPerspective( optScopeAngle + deltas[i], aspectRatio, minDist, maxDist );
      break;
    
    case CAMERA_MODE_REFLEX:
      gluPerspective( reflexScopeAngle, aspectRatio, minDist, maxDist );
      break;
    
    default:
      gluPerspective( defaultFOV, aspectRatio, minDist, maxDist );
      break;
  }
	
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	gluLookAt( cam.pos[0], cam.pos[1], cam.pos[2], cam.lookat[0], cam.lookat[1], cam.lookat[2], cam.upvec[0], cam.upvec[1], cam.upvec[2] );

  // --- ---

  light_setup();
  
  skybox_draw();
  
  environment_draw( timeDiff );

  landscape_draw();
  
  targets_draw( timeDiff );
  
  objects_draw();
  
  trees_draw();
  
  rain_draw( timeDiff );
  
  cameraMovement_draw( timeDiff );

  display_drawEmpty();

  rifle_draw( timeDiff );
  
  display_drawFPS( timeDiff );

  // --- ---
  
  glFlush();
}

void display_drawEmpty()
{
	if(display_showEmpty == 1)
	{
		glPushAttrib(GL_ENABLE_BIT);
		
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		
		glBegin(GL_LINES);
			glColor3f(1.0, 0.0, 0.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(20.0, 0.0, 0.0);
			glColor3f(0.0, 1.0, 0.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, 20.0, 0.0);
			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, 0.0, 20.0);
			glColor3f(1.0, 1.0, 1.0);
		glEnd();
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture_labels);
		glDisable(GL_CULL_FACE);
	
		glBegin(GL_QUADS);
			glTexCoord2d(0, 0.2); glVertex3f(0.0 + 17.0, 0.0, 0.0);
			glTexCoord2d(0.5, 0.2); glVertex3f(3.0 + 17.0, 0.0, 0.0);
			glTexCoord2d(0.5, 0); glVertex3f(3.0 + 17.0, 1.5, 0.0);
			glTexCoord2d(0, 0); glVertex3f(0.0 + 17.0, 1.5, 0.0);
		glEnd();
	
		glBegin(GL_QUADS);
			glTexCoord2d(0, 0.21); glVertex3f(-1.5, 0.0 + 17.0, 0.0);
			glTexCoord2d(0.5, 0.21); glVertex3f(-1.5, 3.0 + 17.0, 0.0);
			glTexCoord2d(0.5, 0.37); glVertex3f(0.0, 3.0 + 17.0, 0.0);
			glTexCoord2d(0, 0.37); glVertex3f(0.0, 0.0 + 17.0, 0.0);
		glEnd();
	
		glBegin(GL_QUADS);
			glTexCoord2d(0.5, 0.55); glVertex3f(0.0, 0.0, 0.0 + 17.0);
			glTexCoord2d(0, 0.55); glVertex3f(0.0, 0.0, 3.0 + 17.0);
			glTexCoord2d(0, 0.37); glVertex3f(0.0, 1.5, 3.0 + 17.0);
			glTexCoord2d(0.5, 0.37); glVertex3f(0.0, 1.5, 0.0 + 17.0);
		glEnd();
		
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);

		glPushMatrix();
		glLoadIdentity();
		glColor3f(1,1,1);
		glBegin(GL_LINES);
			glVertex3f(-0.02, 0, -1.001);
			glVertex3f(0.02, 0, -1.001);

			glVertex3f(0, -0.02, -1.001);
			glVertex3f(0, 0.02, -1.001);
		glEnd();
		glPopMatrix();

		glPopAttrib();
	}
}

void display_setShowEmpty(int state)
{
	if(state == 0)
	{
		display_showEmpty = 0;
	}
	else if(state == 1)
	{
		display_showEmpty = 1;
	}
	else
	{
		if(display_showEmpty == 0)
			display_showEmpty = 1;
		else
			display_showEmpty = 0;
	}
}

// change sh 2006-03-16 begin
void display_drawFPS( double timeDiff )
{
  if( display_showFPS == 1 )
  {
    int i;
    float minFPS = 10000.0, maxFPS = 0.0, averageFPS = 0.0;
    fpsBuffer[fpsBufferCursor] = 1.0/timeDiff;
    fpsBufferCursor++;
  
    if( fpsBufferCursor >= FPS_BUFFER_SIZE )
    {
      fpsBufferCursor = 0;
    }
  
    for( i = 0; i < FPS_BUFFER_SIZE; i++ )
    {
      if(fpsBuffer[i] < minFPS)
        minFPS = fpsBuffer[i];
      
      if(fpsBuffer[i] > maxFPS)
        maxFPS = fpsBuffer[i];
        
      averageFPS += fpsBuffer[i];
    }
  
    averageFPS /= FPS_BUFFER_SIZE;
  
    glPushAttrib( GL_ENABLE_BIT );
    glDisable( GL_TEXTURE_2D );
    glDisable( GL_LIGHTING );
    glDisable( GL_BLEND );
    
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D( -0.5f, 0.5f, -0.5f, 0.5f );  // Eine 2D Projektionsmatrix erstellen
    
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();

    glRasterPos2f(0, 0);
    glBitmap( 0, 0, 0, 0, (float)display_width / 2.0 - 140.0, (float)display_height / 2.0 - 14.0, NULL );

    gdv_os_drawBitmappedText("FPS: current %.1f\n" \
                 "     average %.1f\n" \
                 "     min     %.1f\n" \
                 "     max     %.1f", 1.0/timeDiff, averageFPS, minFPS, maxFPS);
                 
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();

    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
    
    glPopAttrib();
  }
}
// change sh 2006-03-16 end

void display_setShowFPS(int state)
{
	if(state == 0)
	{
		display_showFPS = 0;
	}
	else if(state == 1)
	{
		display_showFPS = 1;
	}
	else
	{
		if(display_showFPS == 0)
			display_showFPS = 1;
		else
			display_showFPS = 0;
	}
}
