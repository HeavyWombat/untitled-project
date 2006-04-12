#ifndef TREES_H_
#define TREES_H_

typedef struct
{
  float p[3];
  int type;
} tree;

void trees_init( void );
void trees_draw( void );
int compareDist( const void* t0, const void* t1 );

#endif /*TREES_H_*/
