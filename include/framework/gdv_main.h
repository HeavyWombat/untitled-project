#ifndef GDV_MAIN_H_
#define GDV_MAIN_H_

#include <GL/gl.h>
#include <GL/glu.h>

#define GDV_PROJECT_NAME "untitled project"

#define GAME_MODE_GEAR_UP 0
#define GAME_MODE_SPLASH 1
#define GAME_MODE_DRAW 42
#define GAME_MODE_HELP 0xF1

extern GLenum polygoneMode;
extern char gdv_key_states[0xFFF];
extern int gameMode;

extern int mouseButtons[3];
extern int mousePos[2];

#endif /*GDV_MAIN_H_*/
