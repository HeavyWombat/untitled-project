#include<GL/gl.h>
#include<GL/glu.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#include<gdv_os.h>
#include<camera.h>
#include<skybox.h>
#include<gdv_resources.h>
#include<display.h>
#include<stadium.h>
#include<light.h>
#include<landscape.h>

long lastMousePos[2] = {0, 0};

GLenum polygoneMode = GL_FILL;

char gdv_key_states[0xFFF];

double gdv_lasttime = 0;
char gdv_title_buffer[256];

#define FPS_BUFFER_SIZE 1000
float fpsBuffer[FPS_BUFFER_SIZE] = {0};
int fpsBufferCursor = 0;

float speeds[6] = {1,1,1,1,1,1};

int gdv_mainloop();
int gdv_main_init();
void gdv_windowEventFunction(GDV_BaseEvent* event);
void gdv_mouseEventFunction(GDV_BaseEvent* event);
void gdv_keyEventFunction(GDV_BaseEvent* event);

int main()
{
	int rc;

	memset(&gdv_key_states, 0, 256);

	rc = gdv_os_init();

	gdv_os_registerMainloopFunction(gdv_mainloop);
	gdv_os_registerWindowEventFunction(gdv_windowEventFunction);
	gdv_os_registerMouseEventFunction(gdv_mouseEventFunction);
	gdv_os_registerKeyEventFunction(gdv_keyEventFunction);

	rc = gdv_os_openWindow("GDV Fussball", 600, 400);
	
	gdv_os_showCursor(0);
	//gdv_os_setFullscreen();

	gdv_os_setMousePosition((float)display_width/2.0, (float)display_height/2.0, 0, &lastMousePos[0], &lastMousePos[1]);

	gdv_main_init();
	
	rc = gdv_os_runMainloop();

	return(0);
}

int gdv_main_init()
{
	int rc = 0;
	
	gdv_resources_loadImages();

	glShadeModel(GL_SMOOTH);
	
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glDisable(GL_TEXTURE_2D);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	
	glEnable(GL_NORMALIZE);
	glEnable(GL_POLYGON_OFFSET_FILL);
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	
	glPolygonMode(GL_FRONT_AND_BACK, polygoneMode);

	camera_init();
	skybox_init();
	stadium_init();
	light_init();
	landscape_init();

	return(rc);
}

int gdv_mainloop()
{
	int i;
	float minFPS = 1000.0, maxFPS = 0.0, averageFPS = 0.0;
	double currentTime = gdv_os_getCurrentTime();
	double timeDiff = currentTime - gdv_lasttime;
	gdv_lasttime = currentTime;

	float acceleration = 40.0;

	if(gdv_key_states['w'])
	{
		speeds[0] += acceleration * timeDiff;
		
		if(speeds[0] > 400.0) speeds[0] = 400.0;

		camera_moveForward(timeDiff * speeds[0]);
	}
	else
	{
		speeds[0] = 1;
	}
	
	if(gdv_key_states['s'])
	{
		speeds[1] += acceleration * timeDiff;
		
		if(speeds[1] > 400.0) speeds[1] = 400.0;
	
		camera_moveBackward(timeDiff * speeds[1]);
	}
	else
	{
		speeds[1] = 1;
	}

	if(gdv_key_states['a'])
	{
		speeds[2] += acceleration * timeDiff;
		
		if(speeds[2] > 400.0) speeds[2] = 400.0;
	
		camera_strafeLeft(timeDiff * speeds[2]);
	}
	else
	{
		speeds[2] = 1;
	}

	if(gdv_key_states['d'])
	{
		speeds[3] += acceleration * timeDiff;
		
		if(speeds[3] > 400.0) speeds[3] = 400.0;
	
		camera_strafeRight(timeDiff * speeds[3]);
	}
	else
	{
		speeds[3] = 1;
	}

	if(gdv_key_states[GDV_KEY_PAGE_UP])
	{
		speeds[4] += acceleration * timeDiff;
		
		if(speeds[4] > 400.0) speeds[4] = 400.0;
	
		camera_moveUp(timeDiff * speeds[4]);
	}
	else
	{
		speeds[4] = 1;
	}

	if(gdv_key_states[GDV_KEY_PAGE_DOWN])
	{
		speeds[5] += acceleration * timeDiff;
		
		if(speeds[5] > 400.0) speeds[5] = 400.0;
	
		camera_moveDown(timeDiff * speeds[5]);
	}
	else
	{
		speeds[5] = 1;
	}

	fpsBuffer[fpsBufferCursor] = 1.0/timeDiff;
	fpsBufferCursor++;
	if(fpsBufferCursor >= FPS_BUFFER_SIZE)
	{
		fpsBufferCursor = 0;
	}

	for(i = 0; i < FPS_BUFFER_SIZE; i++)
	{
		if(fpsBuffer[i] < minFPS) minFPS = fpsBuffer[i];
		if(fpsBuffer[i] > maxFPS) maxFPS = fpsBuffer[i];
		averageFPS += fpsBuffer[i];
	}

	averageFPS /= FPS_BUFFER_SIZE;

	sprintf(gdv_title_buffer, "GDV - FPS: current %.1f - average %.1f - min %.1f - max %.1f", 1.0/timeDiff, averageFPS, minFPS, maxFPS);
	gdv_os_setWindowTitle(gdv_title_buffer);

	display_drawScene(timeDiff);

	return(0);
}

void gdv_windowEventFunction(GDV_BaseEvent* event)
{
	if(event->eventType == GDV_EVENT_WINDOW_SIZE)
	{
		display_setSize(((GDV_Event_Window_Size*)event)->width, ((GDV_Event_Window_Size*)event)->height);
	}
}

void gdv_mouseEventFunction(GDV_BaseEvent* event)
{
	float alpha = ((float)((GDV_Event_Mouse*)event)->xPos - (float)lastMousePos[0]) / 200.0;
	float beta = ((float)((GDV_Event_Mouse*)event)->yPos - (float)lastMousePos[1]) / 200.0;

	camera_rotateYaw(-1.0 * alpha);
	camera_rotatePitch(-1.0 * beta);

	gdv_os_setMousePosition((float)display_width/2.0, (float)display_height/2.0, 0, &lastMousePos[0], &lastMousePos[1]);
}

void gdv_keyEventFunction(GDV_BaseEvent* event)
{
	if(event->eventType == GDV_EVENT_KEY_DOWN)
	{
		gdv_key_states[((GDV_Event_Keyboard*)event)->key] = 1;
	}

	if(event->eventType == GDV_EVENT_KEY_UP)
	{
		gdv_key_states[((GDV_Event_Keyboard*)event)->key] = 0;
	}
	
	if(event->eventType == GDV_EVENT_KEY_DOWN)
	{
		switch(((GDV_Event_Keyboard*)event)->key)
		{
			case GDV_KEY_F9:
				gdv_resources_toggleMarkTexture();
				break;

			case GDV_KEY_F10:
				if(polygoneMode == GL_FILL)
				{
					polygoneMode = GL_LINE;
				}
				else if(polygoneMode == GL_LINE)
				{
					polygoneMode = GL_POINT;
				}
				else
				{
					polygoneMode = GL_FILL;
				}
				
				glPolygonMode(GL_FRONT_AND_BACK, polygoneMode);
				
				break;

			case GDV_KEY_F11:
				display_setShowEmpty(-1);
				break;
			
			case GDV_KEY_F12:
				gdv_os_toggleFullscreenWindow();
				gdv_os_setMousePosition((float)display_width/2.0, (float)display_height/2.0, 0, &lastMousePos[0], &lastMousePos[1]);
				break;
		}
	}
}

