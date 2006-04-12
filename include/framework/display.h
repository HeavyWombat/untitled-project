#ifndef __DISPLAY_H
#define __DISPLAY_H

extern int display_width;
extern int display_height;

extern float aspectRatio;
extern float minDist;
extern float maxDist;

void display_init();
void display_setSize( int w, int h );
void display_drawScene( double timeDiff );
void display_drawEmpty();
void display_setShowEmpty( int state );
void display_drawFPS( double timeDiff );
void display_setShowFPS( int state );

#endif
