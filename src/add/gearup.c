#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <gdv_os.h>
#include <gdv_main.h>
#include <display.h>
#include <cameraMovement.h>
#include <camera.h>
#include <gdv_resources.h>
#include <display.h>
#include <light.h>
#include <objecthandler.h>
#include <sound.h>
#include <cameraMovement.h>
#include <landscape.h>
#include <skybox.h>
#include <trees.h>
#include <objects.h>
#include <selection.h>
#include <targets.h>
#include <text.h>
#include <ttext.h>
#include <help.h>
#include <splash.h>
#include <hud.h>
#include <rifle.h>
#include <rain.h>
#include <environment.h>
#include <gearup.h>

// --- ---

static const int initSize = 18;
static initStruct init[initSize];

static int offset = 0;

// --- ---

// --- ---

void gearup_init( void )
{
  int idx = 0;
  
  init[idx].status = 0;
  init[idx].text = "Textsystem wird initialisiert";
  init[idx].function = ttext_init;
  idx++;
  
  init[idx].status = 0;
  init[idx].text = "Laden der Blender Objekte";
  init[idx].function = objecthandler_init;
  idx++;
  
  init[idx].status = 0;
  init[idx].text = "Laden aller Ressourcen";
  init[idx].function = gdv_resources_loadImages;
  idx++;
  
  init[idx].status = 0;
  init[idx].text = "Initialisierung des Audiosystems";
  init[idx].function = sound_init;
  idx++;
  
  init[idx].status = 0;
  init[idx].text = "Generierung der Zielscheiben";
  init[idx].function = targets_init;
  idx++;
  
  init[idx].status = 0;
  init[idx].text = "Setze die Kameraparameter";
  init[idx].function = camera_init;
  idx++;
  
  init[idx].status = 0;
  init[idx].text = "Initialisieren der Kamerabewegung";
  init[idx].function = cameraMovement_init;
  idx++;
  
  init[idx].status = 0;
  init[idx].text = "Waffen Einstellungen werden initialisiert";
  init[idx].function = rifle_init;
  idx++;
    
  init[idx].status = 0;
  init[idx].text = "Erzeuge die \"Skybox\"";
  init[idx].function = skybox_init;
  idx++;
  
  init[idx].status = 0;
  init[idx].text = "Lade und Erzeuge die Landschaft";
  init[idx].function = landscape_init;
  idx++;
  
  init[idx].status = 0;
  init[idx].text = "Setzen aller Bäume";
  init[idx].function = trees_init;
  idx++;
  
  init[idx].status = 0;
  init[idx].text = "Objekte werden positioniert";
  init[idx].function = objects_init;
  idx++;
  
  init[idx].status = 0;
  init[idx].text = "Hilfeanzeige wird initalisiert";
  init[idx].function = help_init;
  idx++;
  
  init[idx].status = 0;
  init[idx].text = "Eröffnungsbildschirm wird initalisiert";
  init[idx].function = splash_init;
  idx++;
  
  init[idx].status = 0;
  init[idx].text = "Informationssystem wird initalisiert";
  init[idx].function = hud_init;
  idx++;

  init[idx].status = 0;
  init[idx].text = "Regensystem wird initialisiert";
  init[idx].function = rain_init;
  idx++;

  init[idx].status = 0;
  init[idx].text = "Umgebungssystem wird initalisiert";
  init[idx].function = environment_init;
  idx++;
  
  init[idx].status = 0;
  init[idx].text = "Licht setzen und einschalten";
  init[idx].function = light_init;
  idx++;
  
  // ---
  
  for( int i=0; i<initSize; i++ )
  {
    const int length = strlen( init[i].text );
    
    if( length > offset )
      offset = length;
  }
  
  offset += 2;
}

void gearup_gearup( double timeDiff )
{
  int initMutex = 0;
  
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
  
  int i;
  
  const float offsetZ = -0.5f;
  const float centerY = -0.5f;
  
  const float depth = 1.5f;

  const float width = 1.0f;
  const float height = 0.05f;
  
  const float halfHeight = height * 0.5f;
    
  const float halfW2 = 1.0f;
  const float halfH2 = 1.0f * 0.25f * 3.0f;

  glPushAttrib( GL_ENABLE_BIT );
  glDisable( GL_TEXTURE_2D );
  glDisable( GL_BLEND );
  
  glBegin( GL_QUADS );
  glColor3f( 0.0f, 0.0f, 0.1f );
  glVertex3f( depth, -halfH2, -halfW2 );
  glVertex3f( depth, -halfH2, +halfW2 );
  
  glColor3f( 0.0f, 0.0f, 0.2f );
  glVertex3f( depth, +halfH2, +halfW2 );
  glVertex3f( depth, +halfH2, -halfW2 );
  glEnd();
    
  glBegin( GL_LINE_LOOP );
  glColor3f( 1.0f, 1.0f, 1.0f );
  glVertex3f( depth, -halfH2, -halfW2 );
  
  glColor3f( 0.9f, 0.9f, 0.9f );
  glVertex3f( depth, -halfH2, +halfW2 );
  
  glColor3f( 0.8f, 0.8f, 0.8f );
  glVertex3f( depth, +halfH2, +halfW2 );
  
  glColor3f( 0.7f, 0.7f, 0.7f );
  glVertex3f( depth, +halfH2, -halfW2 );
  glEnd();
  
  glPopAttrib();
  
  glColor3f( 1.0f, 1.0f, 1.0f );
  
  text_setTopOffset( 0.4f );
  text_drawTextLn( 0, "Spiel wird geladen ..." );
  text_nLn( 2 );
  
  for( i=0; i<initSize; i++ )
  {
    text_drawText( 0, init[i].text );
    
    if( init[i].status == 0 )
    { 
      if( initMutex == 0 )
      {
        initMutex = i;
        init[i].function();
        init[i].status = 1;
      }
      
      text_Ln();
    }
    else
    {
      text_drawTextLn( offset, "[ OK ]" );
    }
    
    text_Ln();
  }
  
  text_resetTextOffset();
  
  float w = (float)initMutex * width / ( (float)initSize - 1.0f );
  
  glPushAttrib( GL_ENABLE_BIT );
  glDisable( GL_TEXTURE_2D );
  glDisable( GL_BLEND );
  
  glBegin( GL_QUADS );
  glColor3f( 0.0f, 0.0f, 0.2f );
  glVertex3f( depth, centerY - halfHeight, offsetZ + w );
  glVertex3f( depth, centerY - halfHeight, offsetZ + width );
  glVertex3f( depth, centerY + halfHeight, offsetZ + width );
  glVertex3f( depth, centerY + halfHeight, offsetZ + w );
  
  glColor3f( 0.0f, 0.0f, 0.3f );
  glVertex3f( depth, centerY + halfHeight, offsetZ );
  glVertex3f( depth, centerY - halfHeight, offsetZ );
  glColor3f( 0.0f, 0.0f, 0.9f );
  glVertex3f( depth, centerY - halfHeight, offsetZ + w );
  glVertex3f( depth, centerY + halfHeight, offsetZ + w );
  
  glEnd();
    
  glBegin( GL_LINE_LOOP );
  glColor3f( 1.0f, 1.0f, 1.0f );
  glVertex3f( depth, centerY - halfHeight, offsetZ );
  
  glColor3f( 0.9f, 0.9f, 0.9f );
  glVertex3f( depth, centerY - halfHeight, offsetZ + width );
  
  glColor3f( 0.8f, 0.8f, 0.8f );
  glVertex3f( depth, centerY + halfHeight, offsetZ + width );
  
  glColor3f( 0.7f, 0.7f, 0.7f );
  glVertex3f( depth, centerY + halfHeight, offsetZ );
  glEnd();
  
  glPopAttrib();
  
  glFlush();
  
  if( initMutex == 0 )
    gameMode = GAME_MODE_SPLASH;
    //gameMode = GAME_MODE_DRAW;
}
