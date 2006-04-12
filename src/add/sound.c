#include <stdio.h>
#include <fmod.h>

#include <light.h>
#include <ttext.h>
#include <sound.h>

// --- ---

FMOD_SYSTEM* fmodSystem;
FMOD_CHANNEL* fmodChannel0;
FMOD_CHANNEL* fmodChannel1;
FMOD_CHANNEL* fmodChannel2;

FMOD_SOUND* fmodSoundShoot;
FMOD_SOUND* fmodSoundReload;
FMOD_SOUND* fmodSoundRain;
FMOD_SOUND* fmodSoundThunder;

static int rainSoundRunning = 0;
static int rainSound = 1;

// --- ---

static const char* boolToString( int boolean )
{
  return boolean? "true": "false";
}

// --- ---

void sound_init( void )
{
  FMOD_System_Create( &fmodSystem );
  FMOD_System_Init( fmodSystem, 32, FMOD_INIT_NORMAL, NULL );
  
  FMOD_System_CreateSound( fmodSystem, "data\\shoot.wav", FMOD_HARDWARE | FMOD_LOOP_OFF, 0, &fmodSoundShoot );
  FMOD_System_CreateSound( fmodSystem, "data\\reload.wav", FMOD_HARDWARE | FMOD_LOOP_OFF, 0, &fmodSoundReload );
  
  FMOD_System_CreateSound( fmodSystem, "data\\rain.wav", FMOD_HARDWARE | FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI, 0, &fmodSoundRain );
  FMOD_System_CreateSound( fmodSystem, "data\\thunder.wav", FMOD_HARDWARE | FMOD_LOOP_OFF, 0, &fmodSoundThunder );
}

void sound_run( double timeDiff )
{
  static const double rainResetThreshold = 10.0;
  static double rainReset = 0.0;
  rainReset += timeDiff;
  
  // ---
  
  if( rainSound == 1 )
  {
    if( rainSoundRunning == 0 )
    {
      FMOD_System_PlaySound( fmodSystem, FMOD_CHANNEL_FREE, fmodSoundRain, 0, &fmodChannel1 );
      rainSoundRunning = 1;
    }
    
    float vol = 0.3f;
    FMOD_Channel_SetVolume( fmodChannel1, vol );
  }
  else
  {
    if( rainSoundRunning == 1 )
    {
      FMOD_Channel_Stop( fmodChannel1 );
      rainSoundRunning = 0;
    }
  }
  
  // ---
  
  if( rainReset > rainResetThreshold )
  {
    FMOD_Channel_Stop( fmodChannel1 );
    rainSoundRunning = 0;
    
    rainReset = 0.0;
  }
}

void sound_shootSound( void )
{
  int run;
  FMOD_Channel_IsPlaying( fmodChannel0, &run );
  if( run ) 
    FMOD_Channel_Stop( fmodChannel0 );
  
  
  FMOD_Channel_SetVolume( fmodChannel0, 1.0f );
  FMOD_System_PlaySound( fmodSystem, FMOD_CHANNEL_FREE, fmodSoundShoot, 0, &fmodChannel0 );
}

void sound_reloadSound( void )
{
  int run;
  FMOD_Channel_IsPlaying( fmodChannel0, &run );
  if( run ) 
    FMOD_Channel_Stop( fmodChannel0 );
  
  FMOD_Channel_SetVolume( fmodChannel0, 1.0f );
  FMOD_System_PlaySound( fmodSystem, FMOD_CHANNEL_FREE, fmodSoundReload, 0, &fmodChannel0 );
}

void sound_thunderSound( void )
{ 
  int run;
  FMOD_Channel_IsPlaying( fmodChannel2, &run );
  if( run ) 
    FMOD_Channel_Stop( fmodChannel2 );
  
  FMOD_Channel_SetVolume( fmodChannel2, 1.0f );
  FMOD_System_PlaySound( fmodSystem, FMOD_CHANNEL_FREE, fmodSoundThunder, 0, &fmodChannel2 );
}
