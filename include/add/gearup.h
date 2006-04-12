#ifndef GEARUP_H_
#define GEARUP_H_

typedef struct
{
  int status;
  char* text;
  void (*function)( void );
} initStruct;

void gearup_init( void );
void gearup_gearup( double timeDiff );

#endif /*GEARUP_H_*/
