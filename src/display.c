#include<GL/gl.h>
#include<GL/glu.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include<display.h>
#include<camera.h>
#include<skybox.h>
#include<stadium.h>
#include<gdv_resources.h>
#include<light.h>
#include<landscape.h>

int display_width;
int display_height;

int display_showEmpty = 1;

void display_init()
{
}

void display_setSize(int w, int h)
{
	display_width = w;
	display_height = h;
}

void display_drawScene(double timeDiff)
{
	glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, display_width, display_height);

	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60, (float)display_width/(float)display_height, 1.0, 32000.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(cam.pos[0], cam.pos[1], cam.pos[2],
		      cam.lookat[0], cam.lookat[1], cam.lookat[2],
			  cam.upvec[0], cam.upvec[1], cam.upvec[2]);

	skybox_draw(12000.0);

	light_setup();
	landscape_draw();
	stadium_draw(timeDiff);
	
	display_drawEmpty();
}

void display_drawEmpty()
{
	if(display_showEmpty == 1)
	{
		GDV_GLStates currentStates;
		display_saveCurrentGDVGLStates(&currentStates);
		
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		
		glBegin(GL_LINES);
			glColor3f(1.0, 0.0, 0.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(20.0, 0.0, 0.0);
			glColor3f(0.0, 1.0, 0.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, 20.0, 0.0);
			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, 0.0, 20.0);
			glColor3f(1.0, 1.0, 1.0);
		glEnd();
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture_labels);
		glDisable(GL_CULL_FACE);
	
		glBegin(GL_QUADS);
			glTexCoord2d(0, 0.2); glVertex3f(0.0 + 17.0, 0.0, 0.0);
			glTexCoord2d(0.5, 0.2); glVertex3f(3.0 + 17.0, 0.0, 0.0);
			glTexCoord2d(0.5, 0); glVertex3f(3.0 + 17.0, 1.5, 0.0);
			glTexCoord2d(0, 0); glVertex3f(0.0 + 17.0, 1.5, 0.0);
		glEnd();
	
		glBegin(GL_QUADS);
			glTexCoord2d(0, 0.21); glVertex3f(-1.5, 0.0 + 17.0, 0.0);
			glTexCoord2d(0.5, 0.21); glVertex3f(-1.5, 3.0 + 17.0, 0.0);
			glTexCoord2d(0.5, 0.37); glVertex3f(0.0, 3.0 + 17.0, 0.0);
			glTexCoord2d(0, 0.37); glVertex3f(0.0, 0.0 + 17.0, 0.0);
		glEnd();
	
		glBegin(GL_QUADS);
			glTexCoord2d(0.5, 0.55); glVertex3f(0.0, 0.0, 0.0 + 17.0);
			glTexCoord2d(0, 0.55); glVertex3f(0.0, 0.0, 3.0 + 17.0);
			glTexCoord2d(0, 0.37); glVertex3f(0.0, 1.5, 3.0 + 17.0);
			glTexCoord2d(0.5, 0.37); glVertex3f(0.0, 1.5, 0.0 + 17.0);
		glEnd();
		
		display_restoreGDVGLStates(&currentStates);
	}
}

void display_setShowEmpty(int state)
{
	if(state == 0)
	{
		display_showEmpty = 0;
	}
	else if(state == 1)
	{
		display_showEmpty = 1;
	}
	else
	{
		if(display_showEmpty == 0)
			display_showEmpty = 1;
		else
			display_showEmpty = 0;
	}
}

void display_saveCurrentGDVGLStates(GDV_GLStates *states)
{
	states->is_GL_CULL_FACE = glIsEnabled(GL_CULL_FACE);
	states->is_GL_DEPTH_TEST = glIsEnabled(GL_DEPTH_TEST);
	states->is_GL_TEXTURE_2D = glIsEnabled(GL_TEXTURE_2D);
	states->is_GL_LIGHTING = glIsEnabled(GL_LIGHTING);
	states->is_GL_BLEND = glIsEnabled(GL_BLEND);
}

void display_restoreGDVGLStates(GDV_GLStates *states)
{
	if(states->is_GL_CULL_FACE == GL_TRUE)
	{
		glEnable(GL_CULL_FACE);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}

	if(states->is_GL_DEPTH_TEST == GL_TRUE)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	if(states->is_GL_TEXTURE_2D == GL_TRUE)
	{
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}

	if(states->is_GL_LIGHTING == GL_TRUE)
	{
		glEnable(GL_LIGHTING);
	}
	else
	{
		glDisable(GL_LIGHTING);
	}

	if(states->is_GL_BLEND == GL_TRUE)
	{
		glEnable(GL_BLEND);
	}
	else
	{
		glDisable(GL_BLEND);
	}
}
