#ifndef TARGETS_H_
#define TARGETS_H_

#define TARGETS_COUNT 8

#define TARGET_INVISIBLE 0
#define TARGET_MOVING_DOWN 1
#define TARGET_MOVING_UP 2
#define TARGET_VISIBLE 3

#define TARGET_DEFAULT_TIMEOUT 15.0

typedef struct
{
  float position[3];
  float arc;
  
  int visible;
  double timeOut;
} target;

extern target targets[TARGETS_COUNT];

void targets_init( void );
void targets_draw( double timeDiff );

void targets_drawTarget( int number );

#endif /*TARGETS_H_*/
