#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <gdv_os.h>
#include <gdv_main.h>
#include <camera.h>
#include <gdv_resources.h>
#include <display.h>
#include <light.h>
#include <objecthandler.h>
#include <gearup.h>
#include <sound.h>
#include <cameraMovement.h>
#include <landscape.h>
#include <skybox.h>
#include <trees.h>
#include <objects.h>
#include <selection.h>
#include <targets.h>
#include <splash.h>
#include <text.h>
#include <rifle.h>
#include <help.h>

// --- ---

static int mouseCameraMode = 1;
static long lastMousePos[2] = { 0L, 0L };
static double terminateTimeOut = 0.0;
static double gdv_lasttime = 0.0;

GLenum polygoneMode = GL_FILL;
char gdv_key_states[0xFFF];
int gameMode = GAME_MODE_GEAR_UP;
int backupGameMode = gameMode;

int mouseButtons[3] = { 0, 0, 0 };
int mousePos[2];

// --- ---

static int gdv_mainloop();
static int gdv_main_init();

static void gdv_windowEventFunction( GDV_BaseEvent* event );
static void gdv_mouseEventFunction( GDV_BaseEvent* event );
static void gdv_keyEventFunction( GDV_BaseEvent* event );

// --- ---

int main( int argc, char* argv[] )
{
	int rc;

	memset( &gdv_key_states, 0, 256 );

	rc = gdv_os_init();

	gdv_os_registerMainloopFunction( gdv_mainloop );
	gdv_os_registerWindowEventFunction( gdv_windowEventFunction );
	gdv_os_registerMouseEventFunction( gdv_mouseEventFunction );
	gdv_os_registerKeyEventFunction( gdv_keyEventFunction );

	rc = gdv_os_openWindow( GDV_PROJECT_NAME, 1024, 768 );
	
	if( (argc > 1) && (strncmp(argv[1], "-f", 2) == 0) )
	{
		gdv_os_setFullscreen();
	}

	if( (argc > 2) && (strncmp(argv[1], "-t", 2) == 0) )
	{
		terminateTimeOut = gdv_os_getCurrentTime() + (float)atoi(argv[2]);
	}

	if( (argc > 3) && (strncmp(argv[2], "-t", 2) == 0) )
	{
		terminateTimeOut = gdv_os_getCurrentTime() + (float)atoi(argv[3]);
	}

	if( mouseCameraMode == 1 )
	{
		gdv_os_showCursor(0);
    gdv_os_setMousePosition( (long)(display_width >> 1), (long)(display_height >> 1), 0, &lastMousePos[0], &lastMousePos[1]);
	}
	else
	{
		gdv_os_showCursor(1);
	}

  // gdv_os_setFullscreen();
	gdv_main_init();
	
	rc = gdv_os_runMainloop();

	return 0;
}

static int gdv_main_init()
{
	int rc = 0;
	
	glShadeModel( GL_SMOOTH );
	
	glClearDepth( 1.0f );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	
	glDisable( GL_TEXTURE_2D );

	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	glHint( GL_POINT_SMOOTH_HINT, GL_NICEST );
	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
	
	glEnable( GL_NORMALIZE );
	glEnable( GL_POLYGON_OFFSET_FILL );
	
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );

	glDisable( GL_LIGHTING );
	glDisable( GL_BLEND );
  
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glPolygonMode( GL_FRONT_AND_BACK, polygoneMode );
  
  text_init();
  display_init();
  gearup_init();

	return rc;
}

static int gdv_mainloop( void )
{
	double currentTime = gdv_os_getCurrentTime();
	double timeDiff = currentTime - gdv_lasttime;
	gdv_lasttime = currentTime;

  // ---

  switch( gameMode )
  {
    case GAME_MODE_DRAW:
      cameraMovement_moveCamera( timeDiff );
      sound_run( timeDiff );
      
      if( ( bluring == 1 ) && ( cameraMode == CAMERA_MODE_OPT ) )
      {
        const float aThird = 1.0f / 3.0f;
        
        display_drawScene( aThird * timeDiff );
        glAccum( GL_MULT, aThird );
        glAccum( GL_ACCUM, 1.0f - aThird );
        
        display_drawScene( aThird * timeDiff );
        glAccum( GL_MULT, aThird );
        glAccum( GL_ACCUM, 1.0f - aThird );
        
        display_drawScene( aThird * timeDiff );
        glAccum( GL_MULT, aThird );
        glAccum( GL_ACCUM, 1.0f - aThird );

        glAccum( GL_RETURN, 1.0f );
      }
      else
      {
        display_drawScene( timeDiff );
      }
      break;
    
    case GAME_MODE_GEAR_UP:
      gearup_gearup( timeDiff );
      break;
    
    case GAME_MODE_SPLASH:
      splash_draw( timeDiff );
      break;
      
    case GAME_MODE_HELP:
      cameraMovement_moveCamera( timeDiff );
      help_drawHelp( timeDiff );
      break;
  }
  
  // ---

	if(terminateTimeOut > 0)
	{
		if( (terminateTimeOut - gdv_os_getCurrentTime()) < 0.0 )
		{
			return 1;
		}
	}

	return 0;
}

static void gdv_windowEventFunction( GDV_BaseEvent* event )
{
	if(event->eventType == GDV_EVENT_WINDOW_SIZE)
	{
		display_setSize((int)((GDV_Event_Window_Size*)event)->width, (int)((GDV_Event_Window_Size*)event)->height);
	}
}

static void gdv_mouseEventFunction( GDV_BaseEvent* event )
{
	if( mouseCameraMode != 1 )
    return;
  
  // ---
  
  float alpha = ((float)((GDV_Event_Mouse*)event)->xPos - (float)lastMousePos[0]) / 200.0;
	float beta = ((float)((GDV_Event_Mouse*)event)->yPos - (float)lastMousePos[1]) / 200.0;

  gdv_os_setMousePosition((long)((float)display_width/2.0), (long)((float)display_height/2.0), 0, &lastMousePos[0], &lastMousePos[1]);

  cameraMovement_moveCameraYawPitch( alpha, beta );

  // ---
  
  mousePos[0] = ((GDV_Event_Mouse*)event)->xPos;
  mousePos[1] = ((GDV_Event_Mouse*)event)->yPos;
  
  if( ( cameraMode == CAMERA_MODE_OPT ) || ( cameraMode == CAMERA_MODE_REFLEX ) )
    if( event->eventType == GDV_EVENT_MOUSE_DOWN_LEFT_BUTTON )
      shoot();
  
  // ---
  
  switch( event->eventType )
  {
    case GDV_EVENT_MOUSE_DOWN_LEFT_BUTTON:
      mouseButtons[0] = 1;
      break;

    case GDV_EVENT_MOUSE_DOWN_MIDDLE_BUTTON:
      mouseButtons[1] = 1;
      break;

    case GDV_EVENT_MOUSE_DOWN_RIGHT_BUTTON:
      mouseButtons[2] = 1;
      break;
    
    case GDV_EVENT_MOUSE_UP_LEFT_BUTTON:
      mouseButtons[0] = 0;
      break;

    case GDV_EVENT_MOUSE_UP_MIDDLE_BUTTON:
      mouseButtons[1] = 0;
      break;

    case GDV_EVENT_MOUSE_UP_RIGHT_BUTTON:
      mouseButtons[2] = 0;
      break;
    
    default:
      break;
  }
}

static void gdv_keyEventFunction( GDV_BaseEvent* event )
{
	if( (event->eventType == GDV_EVENT_KEY_DOWN) && (gdv_key_states[((GDV_Event_Keyboard*)event)->key] == 1) )
    return;
  	
  // ---
  
	if(event->eventType == GDV_EVENT_KEY_UP)
	{
		gdv_key_states[((GDV_Event_Keyboard*)event)->key] = 0;

		switch(((GDV_Event_Keyboard*)event)->key)
		{
		}
	}
  
  // ---

	if(event->eventType == GDV_EVENT_KEY_DOWN)
	{
		gdv_key_states[((GDV_Event_Keyboard*)event)->key] = 1;

		switch(((GDV_Event_Keyboard*)event)->key)
		{
      case (int)'2':
        if( cameraMode != CAMERA_MODE_FREE )
          cameraMovement_toggleCameraMode( CAMERA_MODE_REFLEX );
        break;
        
      case (int)'3':
        if( cameraMode != CAMERA_MODE_FREE )
          cameraMovement_toggleCameraMode( CAMERA_MODE_OPT );
        break;
      
      case (int)'1':
        if( cameraMode != CAMERA_MODE_FREE )
          cameraMovement_toggleCameraMode( CAMERA_MODE_WALK );
        break;
      
      case (int)'r':
      case (int)'R':
        rifle_reload();
        break;

      case (int)'t':
      case (int)'T':
        rifle_changeMode( RIFLE_MODE_SAFE );
        break;

      case (int)'g':
      case (int)'G':
        rifle_changeMode( RIFLE_MODE_SINGLE );
        break;

      case (int)'b':
      case (int)'B':
        rifle_changeMode( RIFLE_MODE_MULT );
        break;

      case GDV_KEY_F1:
        if( gameMode == GAME_MODE_HELP )
        {
          gameMode = backupGameMode;
        }
        else
        {
          backupGameMode = gameMode;
          gameMode = GAME_MODE_HELP;
        }
        break;
      
      case GDV_KEY_F2:
        bluring = ( bluring == 0 )? 1: 0;
        break;
      
      case GDV_KEY_F3:
        cameraMovement_toggleFreeLocked();
        break;
      
      case GDV_KEY_F4:
        if( glIsEnabled( GL_LIGHT0 ) )
          glDisable( GL_LIGHT0 );
        else
          glEnable( GL_LIGHT0 );      
        break;
      
      case GDV_KEY_F5:
        if( glIsEnabled( GL_FOG ) )
          glDisable( GL_FOG );
        else
          glEnable( GL_FOG );
        break;
      
			case GDV_KEY_F6:
				gdv_os_createScreenshot();
				break;

			case GDV_KEY_F7:
				mouseCameraMode *= -1;

				if(mouseCameraMode == 1)
				{
					gdv_os_setMousePosition((long)((float)display_width/2.0), (long)((float)display_height/2.0), 0, &lastMousePos[0], &lastMousePos[1]);
					gdv_os_showCursor(0);
				}
				else
				{
					gdv_os_showCursor(1);
				}

				break;

			case GDV_KEY_F8:
				display_setShowFPS(-1);
				break;

			case GDV_KEY_F9:
				gdv_resources_toggleMarkTexture();
				break;

			case GDV_KEY_F10:
				if(polygoneMode == GL_FILL)
				{
					polygoneMode = GL_LINE;
				}
				else if(polygoneMode == GL_LINE)
				{
					polygoneMode = GL_POINT;
				}
				else
				{
					polygoneMode = GL_FILL;
				}
				
				glPolygonMode( GL_FRONT_AND_BACK, polygoneMode );
				
				break;

			case GDV_KEY_F11:
				display_setShowEmpty(-1);
				break;
			
			case GDV_KEY_F12:
				gdv_os_toggleFullscreenWindow();
				if(mouseCameraMode == 1)
				{
					gdv_os_setMousePosition((long)((float)display_width/2.0), (long)((float)display_height/2.0), 0, &lastMousePos[0], &lastMousePos[1]);
				}
				break;
		}
	}
}
