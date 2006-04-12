#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <gdv_main.h>
#include <display.h>
#include <camera.h>
#include <gdv_os.h>
#include <landscape.h>
#include <additionalMath.h>
#include <cameraMovement.h>
#include <hud.h>
#include <ttext.h>
#include <gdv_resources.h>

// --- ---

static int optScopeList;
static int reflexScopeList;

static float crouchHeight = 0.25f;
static float kneeHeight = 1.0f;
static float standHeight = 1.82f;

static float invertMouse = -1.0f;

static const float crouchJitter = 0.0125f;
static const float kneeJitter = 0.025;
static const float calmJitter = 0.05f;
static const float walkAndReflexJitter = 0.5f;

static float jitterFactor = calmJitter;


static float maxAngleXFactor = 6.0f;
static float maxAngleYFactor = 12.0f;

static const float peakTurnAlpha = 0.1f;
static const float peakTurnBeta = 0.1f;

static camera scopeCamera;
static float speeds[6] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };

static float alphaValue = 42.0f;
static float betaValue = 42.0f;

static float targetOffset = 0.0f;

const float defaultFOV = 60.0f;
const float optScopeAngle = calcMagnifierAngle( 3.0f );
const float reflexScopeAngle = calcMagnifierAngle( 1.0f );

int cameraMode = CAMERA_MODE_FREE;
int changeToMode = CAMERA_MODE_NO_CHANGE;
int playerPosition = PLAYER_POSITION_STAND;
int bluring = 0;

float height = standHeight;

// --- ---

static void cameraMovement_copyCameraSetup( camera& src, camera& dest );
static void printStat( double timeDiff );
static int checkAndModify( int character, int arrayIndex, double timeDiff, float threshold, float acceleration );
static void adjustHeight( double timeDiff );
static void adjustPosition( void );

// --- ---

static void cameraMovement_copyCameraSetup( camera& src, camera& dest )
{
  dest.lookat[0] = src.lookat[0];
  dest.lookat[1] = src.lookat[1];
  dest.lookat[2] = src.lookat[2];
  
  dest.pos[0] = src.pos[0];
  dest.pos[1] = src.pos[1];
  dest.pos[2] = src.pos[2];
  
  dest.upvec[0] = src.upvec[0];
  dest.upvec[1] = src.upvec[1];
  dest.upvec[2] = src.upvec[2];    
}

static void printStat( double timeDiff )
{
  const float leftOffset = -0.5f * (float)display_width;
  const float topOffset = 0.5f * (float)display_height;
  
  const float left = 0.0f;
  
  float top = 10.0f;
  float step = 12.0f;
  
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

  glRasterPos2f( 0, 0 );
  glBitmap( 0, 0, 0, 0, leftOffset + left, topOffset - top, NULL );

  if( cameraMode == CAMERA_MODE_FREE ) 
    gdv_os_drawBitmappedText( "Free Camera" );
  else
    gdv_os_drawBitmappedText( "Locked Camera" );

  top += step;
  glRasterPos2f( 0, 0 );
  glBitmap( 0, 0, 0, 0, leftOffset + left, topOffset - top, NULL );
  gdv_os_drawBitmappedText( "X ... pos = %.2f, lookAt = %.2f and upVec = %.2f", cam.pos[0], cam.lookat[0], cam.upvec[0] );

  top += step;
  glRasterPos2f( 0, 0 );
  glBitmap( 0, 0, 0, 0, leftOffset + left, topOffset - top, NULL );
  gdv_os_drawBitmappedText( "Y ... pos = %.2f, lookAt = %.2f and upVec = %.2f", cam.pos[1], cam.lookat[1], cam.upvec[1] );

  top += step;
  glRasterPos2f( 0, 0 );
  glBitmap( 0, 0, 0, 0, leftOffset + left, topOffset - top, NULL );
  gdv_os_drawBitmappedText( "Z ... pos = %.2f, lookAt = %.2f and upVec = %.2f", cam.pos[2], cam.lookat[2], cam.upvec[2] );

  top += step;
  glRasterPos2f( 0, 0 );
  glBitmap( 0, 0, 0, 0, leftOffset + left, topOffset - top, NULL );
  gdv_os_drawBitmappedText( "timeDiff = %f", timeDiff );

  top += step;
  glRasterPos2f( 0, 0 );
  glBitmap( 0, 0, 0, 0, leftOffset + left, topOffset - top, NULL );
  gdv_os_drawBitmappedText( "alpha = %f", alphaValue );

  top += step;
  glRasterPos2f( 0, 0 );
  glBitmap( 0, 0, 0, 0, leftOffset + left, topOffset - top, NULL );
  gdv_os_drawBitmappedText( "beta = %f", betaValue );  
  
  float eX[3] = { 1.0f, 0.0f, 0.0f  };
  float lA[3];
  float angle;
  
  vecSub( cam.lookat, cam.pos, lA );
  
  top += step;
  glRasterPos2f( 0, 0 );
  glBitmap( 0, 0, 0, 0, leftOffset + left, topOffset - top, NULL );
  gdv_os_drawBitmappedText( "lA ... x = %.2f, y = %.2f and z = %.2f", lA[0], lA[1], lA[2] );
  
  vecSub( cam.lookat, cam.pos, lA );
  lA[1] = 0.0f;
  angle = calcAngle( lA, eX ) * 180.0f * M_1_PI;
  
  top += step;
  glRasterPos2f( 0, 0 );
  glBitmap( 0, 0, 0, 0, leftOffset + left, topOffset - top, NULL );
  gdv_os_drawBitmappedText( "angleX = %f", angle );

  vecSub( cam.lookat, cam.pos, lA );
  lA[2] = 0.0f;
  angle = calcAngle( lA, eX ) * 180.0f * M_1_PI;
  
  top += step;
  glRasterPos2f( 0, 0 );
  glBitmap( 0, 0, 0, 0, leftOffset + left, topOffset - top, NULL );
  gdv_os_drawBitmappedText( "angleY = %f", angle );

  glPopMatrix();
  glPopAttrib();
}

static int checkAndModify( int character, int arrayIndex, double timeDiff, float threshold, float acceleration )
{
  int resultValue = 0;
  
  if( gdv_key_states[character] )
  {
    speeds[arrayIndex] += ( speeds[arrayIndex] > threshold )? 0.0f : acceleration * timeDiff;
    
    switch( arrayIndex )
    {
      case 0:
        camera_moveForward( timeDiff * speeds[arrayIndex] );
        break;
      
      case 1:
        camera_moveBackward( timeDiff * speeds[arrayIndex] );
        break;
      
      case 2:
        camera_strafeLeft( timeDiff * speeds[arrayIndex] );
        break;
      
      case 3:
        camera_strafeRight( timeDiff * speeds[arrayIndex] );
        break;
      
      case 4:
        camera_moveUp( timeDiff * speeds[arrayIndex] );
        break;
      
      case 5:
        camera_moveDown( timeDiff * speeds[arrayIndex] );
        break;
    }
    
    resultValue = 1;
  }
  else
  {
    speeds[arrayIndex] = 1.0f;
  }
  
  return resultValue;
}

static void adjustHeight( double timeDiff )
{
  const float posDelta = 0.05f;
  const float negDelta = -1.0f * posDelta;

  float oH = cam.pos[1];
  float nH = height + getHeightAtPos( cam.pos[0], cam.pos[2] );
  
  float delta = nH - oH;
  float factor = 2.0f;
  
  if( delta > posDelta )
  {
    if( delta > 2.0f )
      factor = 8.0f;
    
    if( delta > 1.0f )
      factor = 4.0f;
    
    cam.pos[1] += factor * timeDiff;
    cam.lookat[1] += factor * timeDiff;
    
    if( cam.pos[1] > nH )
    {
      cam.pos[1] += delta;
      cam.lookat[1] += delta;
    }
    
    return;
  }
  
  if( delta < negDelta )
  {
    if( delta < -2.0f )
      factor = 8.0f;
    
    if( delta < -1.0f )
      factor = 4.0f;

    cam.pos[1] -= factor * timeDiff;
    cam.lookat[1] -= factor * timeDiff;
    
    if( cam.pos[1] < nH )
    {
      cam.pos[1] += delta;
      cam.lookat[1] += delta;
    }
    
    return;
  }
  
  // ---
    
  /*
  float adjustedHeight = height + getHeightAtPos( cam.pos[0], cam.pos[2] );
  float delta = adjustedHeight - cam.pos[1];
  
  cam.pos[1] = adjustedHeight;
  cam.lookat[1] += delta;
  */  
}

static void adjustPosition( void )
{
}

// --- ---

/**
 * 
 */
void cameraMovement_init( void )
{
  srand( (unsigned int)gdv_os_getCurrentTime() );
  
  scopeCamera.pos[0] = 0.0f;
  scopeCamera.pos[1] = height;
  scopeCamera.pos[2] = 0.0f;
  
  scopeCamera.lookat[0] = 10.0f;
  scopeCamera.lookat[1] = height;
  scopeCamera.lookat[2] = 0.0f;
  
  scopeCamera.upvec[0] = 0.0f;
  scopeCamera.upvec[1] = 1.0f;
  scopeCamera.upvec[2] = 0.0f;
  
  // ---
  
  optScopeList = glGenLists( 1 );
  glNewList( optScopeList, GL_COMPILE );
  
  glNormal3f( -1.0f, 0.0f, 0.0f );
  
  glBegin( GL_QUADS );
  glTexCoord2d( 0, 1 );
  glVertex2i( -1, -1 );
  
  glTexCoord2d( 1, 1 );
  glVertex2i( 1, -1 );
  
  glTexCoord2d( 1, 0 );
  glVertex2i( 1, 1 );
  
  glTexCoord2d( 0, 0 );
  glVertex2i( -1, 1 );
  glEnd();
  
  glEndList();
  
  // ---
  
  reflexScopeList = glGenLists( 1 );
  glNewList( reflexScopeList, GL_COMPILE );
  
  glNormal3f( -1.0f, 0.0f, 0.0f );
    
  glBegin( GL_QUADS );
  glTexCoord2d( 0, 1 );
  glVertex2i( -1, -1 );
  
  glTexCoord2d( 1, 1 );
  glVertex2i( 1, -1 );
  
  glTexCoord2d( 1, 0 );
  glVertex2i( 1, 1 );
  
  glTexCoord2d( 0, 0 );
  glVertex2i( -1, 1 );
  glEnd();
  
  glEndList();
  
  // ---
  
  cameraMovement_toggleFreeLocked();
}

/**
 * 
 */
void cameraMovement_draw( double timeDiff )
{
  if( cameraMode != CAMERA_MODE_FREE )
  {
    // jitter
    camera_rotatePitch( timeDiff * jitterFactor * randomFloatNegPos() );
    camera_rotateYaw( timeDiff * jitterFactor * randomFloatNegPos() );

    // ---
  
    glPushAttrib( GL_ENABLE_BIT );
    
    glDisable( GL_DEPTH_TEST );
    
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_BLEND );
    
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();
    
    // ---
  
    static const float angleDiff = 4.0f;
    static float offset = 0.0f;
    
    switch( cameraMode )
    {
      case CAMERA_MODE_WALK:
        offset = 0.0f;
        break;
            
      case CAMERA_MODE_OPT:
        offset = 4.0f;
        break;
      
      case CAMERA_MODE_REFLEX:
        offset = 2.0f;
        break;
      
      case CAMERA_MODE_CHANGE:
        
        if( targetOffset > offset )
        {
          offset += angleDiff * timeDiff;
          
          if( offset > targetOffset )
          {
            offset = targetOffset;
            cameraMode = changeToMode;
          }
        }
        else
        {
          offset -= angleDiff * timeDiff;
          
          if( offset < targetOffset )
          {
            offset = targetOffset;
            cameraMode = changeToMode;
          }
        }
        break;
    }
    
    glTranslatef( 0.0f, offset, 0.0f );
    
    glTranslatef( 0.0f, -2.0f, 0.0f );
    glBindTexture( GL_TEXTURE_2D, texture_reflexScope );
    glCallList( reflexScopeList );

    glTranslatef( 0.0f, -2.0f, 0.0f );
    glBindTexture( GL_TEXTURE_2D, texture_optScope );
    glCallList( optScopeList );
    
    // ---
    
    glPopMatrix();
    glPopMatrix();
    glPopAttrib();
    
    hud_draw( timeDiff );
  }
  
  // ---
  
  if( cameraMode != CAMERA_MODE_FREE )
  {
    adjustHeight( timeDiff );
    adjustPosition();
  }
  
  // ---
  
  // printStat( timeDiff );
}

void cameraMovement_toggleFreeLocked( void )
{
  switch( cameraMode )
  {
    case CAMERA_MODE_FREE:
      cameraMode = CAMERA_MODE_WALK;
      break;
      
    case CAMERA_MODE_WALK:
      cameraMode = CAMERA_MODE_FREE;
      break;
    
    default:
      return;
  }
  
  camera swpCamera;
  cameraMovement_copyCameraSetup( cam, swpCamera );
  cameraMovement_copyCameraSetup( scopeCamera, cam );
  cameraMovement_copyCameraSetup( swpCamera, scopeCamera );
}

/**
 * 
 */
void cameraMovement_toggleCameraMode( int nextMode )
{
  cameraMode = CAMERA_MODE_CHANGE;
  changeToMode = nextMode;
  
  switch( nextMode )
  {
    case CAMERA_MODE_WALK:
      targetOffset = 0.0f;
      break;
    
    case CAMERA_MODE_OPT:
      targetOffset = 4.0f;
      break;
    
    case CAMERA_MODE_REFLEX:
      targetOffset = 2.0f;
      break;
  }
}

void cameraMovement_moveCameraYawPitch( float alpha, float beta )
{
  switch( cameraMode )
  {
    case CAMERA_MODE_WALK:
    case CAMERA_MODE_FREE:
    case CAMERA_MODE_CHANGE:
    case CAMERA_MODE_REFLEX:
      camera_rotateYaw( invertMouse * alpha );
      camera_rotatePitch( invertMouse * beta );
      break;

    case CAMERA_MODE_OPT:
    
      float eX[3] = { 1.0f, 0.0f, 0.0f };
      float lA[3];
      
      // ---

      if( alpha > peakTurnAlpha )
        alpha = peakTurnAlpha;
        
      if( alpha < -peakTurnAlpha )
        alpha = -peakTurnAlpha;

      vecSub( cam.lookat, cam.pos, lA );
      lA[1] = 0.0f;
      
      if( ( ( lA[2] > 0.0f ) && ( alpha > 0.0f ) ) || ( ( lA[2] < 0.0f ) && ( alpha < 0.0f ) ) )
        alpha *= cos( maxAngleXFactor * ( calcAngle( lA, eX ) - M_PI ) );
         
      camera_rotateYaw( invertMouse * alpha );
      
      // ---
      
      if( beta > peakTurnBeta )
        beta = peakTurnBeta;
        
      if( beta < -peakTurnBeta )
        beta = -peakTurnBeta;

      vecSub( cam.lookat, cam.pos, lA );
      lA[2] = 0.0f;
      
      if( ( ( lA[1] > 0.0f ) && ( beta < 0.0f ) ) || ( ( lA[1] < 0.0f ) && ( beta > 0.0f ) ) )
        beta *= cos( maxAngleYFactor * ( calcAngle( lA, eX ) - M_PI ) );
                  
      camera_rotatePitch( invertMouse * beta );
      
      break;
  }
  
  alphaValue = equals( alpha, 0.0f ) ? alphaValue: alpha;
  betaValue = equals( beta, 0.0f ) ? betaValue: beta;
}

void cameraMovement_moveCamera( double timeDiff )
{
  float threshold;
  float acceleration;
  
  switch( cameraMode )
  {
    case CAMERA_MODE_FREE:
      threshold = 400.0f;
      acceleration = 40.0f;
      
      checkAndModify( (int)'w', 0, timeDiff, threshold, acceleration );
      checkAndModify( (int)'s', 1, timeDiff, threshold, acceleration );
      checkAndModify( (int)'a', 2, timeDiff, threshold, acceleration );
      checkAndModify( (int)'d', 3, timeDiff, threshold, acceleration );
      checkAndModify( GDV_KEY_UP, 4, timeDiff, threshold, acceleration );
      checkAndModify( GDV_KEY_DOWN, 5, timeDiff, threshold, acceleration );
      break;
   
    case CAMERA_MODE_WALK:
      threshold = 10.0f;
      acceleration = 4.0f;
    case CAMERA_MODE_REFLEX:
      threshold = 5.0f;
      acceleration = 2.0f;
      
      int movement = 0;
      
      float posHeight = cam.pos[1];
      float loAHeight = cam.lookat[1];
      
      movement |= checkAndModify( (int)'w', 0, timeDiff, threshold, acceleration );
      movement |= checkAndModify( (int)'s', 1, timeDiff, threshold, acceleration );
      movement |= checkAndModify( (int)'a', 2, timeDiff, threshold, acceleration );
      movement |= checkAndModify( (int)'d', 3, timeDiff, threshold, acceleration );
      
      cam.pos[1] = posHeight;
      cam.lookat[1] = loAHeight;
      
      // ---
      
      if( gdv_key_states[ (int)'f' ] || gdv_key_states[ (int)'F' ] )
      {
        playerPosition = PLAYER_POSITION_KNEE;
      }
      else
      {
        if( gdv_key_states[ (int)'c' ] || gdv_key_states[ (int)'C' ] )
        {
          playerPosition = PLAYER_POSITION_CROUCH;
        }
        else
        {
          playerPosition = PLAYER_POSITION_STAND;
        }
      }
      
      // ---      
      
      if( movement )
      {
        
        switch( playerPosition )
        {
          case PLAYER_POSITION_STAND:
            if( cameraMode == CAMERA_MODE_REFLEX )
            {
              jitterFactor = walkAndReflexJitter;
            }
            break;
        }
        
      }
      else
      {
        
        switch( playerPosition )
        {
          case PLAYER_POSITION_STAND:
            height = standHeight;
            jitterFactor = calmJitter;
            break;

          case PLAYER_POSITION_KNEE:
            height = kneeHeight;
            jitterFactor = kneeJitter;
            break;

          case PLAYER_POSITION_CROUCH:
            height = crouchHeight;
            jitterFactor = crouchJitter;
            break;
        }
        
      }
      
      break;    
  }
}
