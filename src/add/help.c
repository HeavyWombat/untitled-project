#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

#include <gdv_main.h>
#include <gdv_resources.h>
#include <camera.h>
#include <display.h>
#include <text.h>
#include <cameraMovement.h>
#include <light.h>
#include <landscape.h>
#include <skybox.h>
#include <trees.h>
#include <objects.h>
#include <rain.h>
#include <rifle.h>
#include <environment.h>
#include <targets.h>

#include <help.h>

// --- ---

static int quadList;

static const float width = 1.0f;
static const float height = width;

static const float halfWidth = width * 0.5f;
static const float halfHeight = height * 0.5f;

static int w = 256;

// --- ---

static void drawScene( double timeDiff, GLenum polyMode )
{
  glPolygonMode( GL_FRONT_AND_BACK, polyMode );

  glClearColor( 0.0f, 0.0f, 0.2f, 1.0f );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glViewport( 0, 0, w, w );

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  gluPerspective( defaultFOV, 1.0f, minDist, maxDist );
  
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  gluLookAt( cam.pos[0], cam.pos[1], cam.pos[2], cam.lookat[0], cam.lookat[1], cam.lookat[2], cam.upvec[0], cam.upvec[1], cam.upvec[2] );

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
  
  // ---
  
  glPolygonMode( GL_FRONT_AND_BACK, polygoneMode );
}

// --- ---

void help_init( void )
{
  quadList = glGenLists( 1 );
  glNewList( quadList, GL_COMPILE );
  
  glPushAttrib( GL_ENABLE_BIT );

  glEnable( GL_TEXTURE_2D );
  glDisable( GL_BLEND );
  
  glBegin( GL_QUADS );
  glTexCoord2i( 0, 0 );
  glVertex3f( 0.0f, -halfHeight, -halfWidth );
  glTexCoord2i( 1, 0 );
  glVertex3f( 0.0f, -halfHeight, +halfWidth );
  glTexCoord2i( 1, 1 );
  glVertex3f( 0.0f, +halfHeight, +halfWidth );
  glTexCoord2i( 0, 1 );
  glVertex3f( 0.0f, +halfHeight, -halfWidth );
  glEnd();
  

  glDisable( GL_TEXTURE_2D );
  glEnable( GL_BLEND );
  
  glBegin( GL_LINE_LOOP );
  glColor4f( 1.0f, 1.0f, 1.0f, 0.0f );
  glVertex3f( 0.0f, -halfHeight, -halfWidth );
  glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
  glVertex3f( 0.0f, -halfHeight, +halfWidth );
  glColor4f( 1.0f, 1.0f, 1.0f, 0.0f );
  glVertex3f( 0.0f, +halfHeight, +halfWidth );
  glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
  glVertex3f( 0.0f, +halfHeight, -halfWidth );
  glEnd();
  
  glPopAttrib();
  
  glEndList();
}

void help_drawHelp( double timeDiff )
{
  drawScene( 0.333f * timeDiff, GL_FILL );
  glBindTexture( GL_TEXTURE_2D, texture_help[0] );
  glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 0, 0, w, w, 0 );

  drawScene( 0.333f * timeDiff, GL_LINE );
  glBindTexture( GL_TEXTURE_2D, texture_help[1] );
  glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 0, 0, w, w, 0 );
  
  drawScene( 0.333f * timeDiff, GL_POINT );
  glBindTexture( GL_TEXTURE_2D, texture_help[2] );
  glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 0, 0, w, w, 0 );
  
  // ---
  
  glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glViewport( 0, 0, display_width, display_height );

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  
  gluPerspective( defaultFOV, aspectRatio, minDist, maxDist );
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gluLookAt( 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f );
  
  // ---
  
  glPushAttrib( GL_ENABLE_BIT );
  
  glDisable( GL_LIGHTING );
  
  glPushMatrix();
  
  glPushMatrix();
  
  glTranslatef( 1.0f, 0.08f, 0.0f );
  glRotatef( 22.5f, 0.0f, 0.0f, 1.0f );
  glRotatef( -45.0f, 0.0f, 1.0f, 0.0f );

  glPushMatrix();
  glTranslatef( halfWidth, 0.0f, 0.0f );
  glRotatef( 0.0f, 0.0f, 1.0f, 0.0f );
  glBindTexture( GL_TEXTURE_2D, texture_help[0] );
  glCallList( quadList );
  glPopMatrix();
  
  glPushMatrix();
  glTranslatef( 0.0f, 0.0f, -halfWidth );
  glRotatef( 90.0f, 0.0f, 1.0f, 0.0f );
  glBindTexture( GL_TEXTURE_2D, texture_help[1] );
  glCallList( quadList );
  glPopMatrix();

  glPushMatrix();
  glTranslatef( 0.0f, -halfWidth, 0.0f );
  glRotatef( -90.0f, 0.0f, 0.0f, 1.0f );
  glBindTexture( GL_TEXTURE_2D, texture_help[2] );
  glCallList( quadList );
  glPopMatrix();
  
  glPopMatrix();

  // ---

  const int offset = -10;
  
  text_setDefaults();
  
  text_drawTextLn( offset, GDV_PROJECT_NAME );
  text_nLn( 4 );
  
  text_drawTextLn( offset, "Ziel des Spiels ..." );
  text_drawTextLn( offset, "bis jetzt noch: Einfach die Klappfallscheiben abschießen" );
  text_nLn( 2 );
  
  text_drawTextLn( offset, "Tastaturbelegung:" );
  text_drawTextLn( offset, " F1  ...  Hilfe" );
  text_drawTextLn( offset, " F3  ...  Wechsel zwischen freien Flug und Personensicht" );
  text_drawTextLn( offset, " F4  ...  Dunkelheit ein/aus" );
  text_drawTextLn( offset, " F5  ...  Nebel ein/aus" );
  text_drawTextLn( offset, " F6  ...  Bildschirmabdruck (buggy im Vollbildmodus!)" );
  text_drawTextLn( offset, " F7  ...  Maus entlassen und für die Fensterarbeit freigeben" );
  text_drawTextLn( offset, " F8  ...  Zähler für Bilder pro Sekunde ein/aus" );
  text_drawTextLn( offset, " F9  ...  Wechsel zwischen Testtexturierung und korrekter" );
  text_drawTextLn( offset, "F10  ...  Wechsel zwischen Texturierung, Drahtgitterdarstellung und Punktdarstellung" );
  text_drawTextLn( offset, "F11  ...  Koordinatenkreuz und Zeiger ein/aus" );
  text_drawTextLn( offset, "F12  ...  Vollbildmodus ein/aus" );
  
  text_resetTextOffset();

  // ---
  
  glPopMatrix();
  glPopAttrib();
  
  display_drawFPS( timeDiff );
  
  // ---
  
  glFlush();
}
