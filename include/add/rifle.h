#ifndef RIFLE_H_
#define RIFLE_H_

#define RIFLE_MODE_SAFE 0
#define RIFLE_MODE_SINGLE 1
#define RIFLE_MODE_MULT 2

void rifle_init( void );
void rifle_draw( double timeDiff );
void shoot( void );
void rifle_reload( void );
void rifle_changeMode( int nextMode );

#endif /*RIFLE_H_*/
