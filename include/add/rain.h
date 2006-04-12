#ifndef RAIN_H_
#define RAIN_H_

typedef struct
{
  float pos[3];
  float dist;
  float speed;
} rainDrop;

void rain_init( void );
void rain_draw( double timeDiff );
int rain_compareDist( const void* f0, const void* f1 );

#endif /*RAIN_H_*/
