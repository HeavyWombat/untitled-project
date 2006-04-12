#include <stdio.h>

#include <gdv_os.h>
#include <gdv_main.h>
#include <cameraMovement.h>
#include <sound.h>
#include <selection.h>
#include <ttext.h>
#include <rifle.h>

// --- ---

static int reloading = 0;

static int rifleMode = RIFLE_MODE_SAFE;

static unsigned int reloadCount = 30;
static unsigned int roundCount = 0;

static double reloadTime = 5.0;

// --- ---

// --- ---

void rifle_init( void )
{
}

void rifle_draw( double timeDiff )
{
  if( cameraMode == CAMERA_MODE_FREE )
    return;

  // ---

  static double counter = 0.0;
  
  static float bulletCount = 0.0f;
  
  const int bufSize = 32;
  char buf[bufSize];
  
  // ---
  
  switch( rifleMode )
  {
    case RIFLE_MODE_SAFE:
      ttext_drawText( 0.0f, 0.0f, "G36 gesichert" );
      break;

    case RIFLE_MODE_SINGLE:
      ttext_drawText( 0.0f, 0.0f, "Einzelfeuer" );
      break;

    case RIFLE_MODE_MULT:
      ttext_drawText( 0.0f, 0.0f, "Feuerstoß" );
      
      if( mouseButtons[0] )
      {
        bulletCount += ( 750.0f / 60.0f ) * timeDiff;
        
        if( bulletCount > 1.0f )
          for( ; bulletCount > 1.0f; bulletCount -= 1.0f )
            shoot();
      }
      
      break;
  }
  
  if( reloading )
  {
    counter += timeDiff;
    
    if( counter > reloadTime )
    {
      roundCount = reloadCount;
      sound_reloadSound();
      
      reloading = 0;
      counter = 0.0;
    }
    
    ttext_drawText( 1.0e10f, 1.0e10f, "laden" );
  }
  else
  {
    snprintf( buf, bufSize, "%ux", roundCount );
    ttext_drawText( 1.0e10f, 1.0e10f, buf );
  }
}

void shoot( void )
{
  if( ( rifleMode != RIFLE_MODE_SAFE ) && ( roundCount > 0.0f ) && ( reloading == 0 ) )
  {
    sound_shootSound();
    
    roundCount--;
    
    selection_select( mousePos[0], mousePos[1] );
    
    cameraMovement_moveCameraYawPitch( 0.0f, -0.02f );
  }
}

void rifle_reload( void )
{
  reloading = 1;
}

void rifle_changeMode( int nextMode )
{
  rifleMode = nextMode;
}
