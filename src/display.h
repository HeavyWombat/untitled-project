#ifndef __DISPLAY_H
#define __DISPLAY_H

typedef struct
{
	int is_GL_CULL_FACE;
	int is_GL_DEPTH_TEST;
	int is_GL_TEXTURE_2D;
	int is_GL_LIGHTING;
	int is_GL_BLEND;
} GDV_GLStates;

extern int display_width;
extern int display_height;

void display_init();
void display_setSize(int w, int h);
void display_drawScene(double timeDiff);
void display_drawEmpty();
void display_setShowEmpty(int state);
void display_saveCurrentGDVGLStates(GDV_GLStates *states);
void display_restoreGDVGLStates(GDV_GLStates *states);

#endif
