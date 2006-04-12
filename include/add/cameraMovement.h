#ifndef CAMERAMOVEMENT_H_
#define CAMERAMOVEMENT_H_

#define CAMERA_MODE_NO_CHANGE -1
#define CAMERA_MODE_CHANGE -2
#define CAMERA_MODE_FREE 42
#define CAMERA_MODE_WALK 0
#define CAMERA_MODE_INPOSITION 1
#define CAMERA_MODE_OPT 10
#define CAMERA_MODE_REFLEX 20

#define PLAYER_POSITION_STAND 1
#define PLAYER_POSITION_KNEE 2
#define PLAYER_POSITION_CROUCH 3

extern const float defaultFOV;
extern const float optScopeAngle;
extern const float reflexScopeAngle;
extern int cameraMode;
extern int bluring;

void cameraMovement_toggleFreeLocked( void );
void cameraMovement_toggleCameraMode( int nextMode );

void cameraMovement_init( void );
void cameraMovement_draw( double timeDiff );
void cameraMovement_moveCameraYawPitch( float alpha, float beta );
void cameraMovement_moveCamera( double timeDiff );

#endif /*CAMERAMOVEMENT_H_*/
