#include<GL/gl.h>
#include<GL/glu.h>
#include<GL/glext.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include <fakelib.h>

#include<gdv_os.h>

#include<landscape.h>
#include<gdv_resources.h>

GLuint texture_mark;
GLuint texture_skybox_left;
GLuint texture_skybox_front;
GLuint texture_skybox_right;
GLuint texture_skybox_back;
GLuint texture_skybox_top;
GLuint texture_skybox_bottom;
GLuint texture_labels;
GLuint texture_field;
GLuint texture_net;
GLuint texture_track;

static int gdv_resources_markIsShowing = 0;

static GLuint save_texture_skybox_left;
static GLuint save_texture_skybox_front;
static GLuint save_texture_skybox_right;
static GLuint save_texture_skybox_back;
static GLuint save_texture_skybox_top;
static GLuint save_texture_skybox_bottom;
static GLuint save_texture_labels;
static GLuint save_texture_field;
static GLuint save_texture_net;
static GLuint save_texture_track;


int gdv_resources_loadImages()
{
	fake_lib_t *pFakeLib = NULL;
	fake_image_t *pFakeImage = NULL;
	
	pFakeLib = fake_new();
	
	fake_dar_add_dir(pFakeLib, "../data");
	fake_dar_add_dir(pFakeLib, "data");

	pFakeImage = fake_imageFromPNG(pFakeLib, "mark.png");
	fake_image_toRGBA8888(pFakeLib, pFakeImage);
	texture_mark = gdv_resources_textureFromFakeImage(pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0);
	fake_image_delete(pFakeLib, &pFakeImage);

	pFakeImage = fake_imageFromPNG(pFakeLib, "labels.png");
	fake_image_toRGBA8888(pFakeLib, pFakeImage);
	texture_labels = gdv_resources_textureFromFakeImage(pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0);
	fake_image_delete(pFakeLib, &pFakeImage);


	// TODO: Load and generate your textures here


	return(0);
}

GLuint gdv_resources_textureFromFakeImage(fake_lib_t *pfl, fake_image_t *fi, GLint minFilter, GLint magFilter, GLint wrapS, GLint wrapT, GLint level)
{
	GLuint texid = 0;

	if(fi)
	{
		if(level == 0)
		{
			glGenTextures(1, &texid);                                               

			glBindTexture(GL_TEXTURE_2D, texid);                                    
			
			glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, fi->width, fi->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, fi->data);
		}
		else if(level > 0)
		{
			glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, fi->width, fi->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, fi->data);
		}
		else
		{
			glGenTextures(1, &texid);                                               

			glBindTexture(GL_TEXTURE_2D, texid);                                    
			
			gluBuild2DMipmaps(GL_TEXTURE_2D, 4, fi->width, fi->height, GL_RGBA, GL_UNSIGNED_BYTE, fi->data);
		}
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
	}
	else
	{
		printf("Error loading image\n");
		return(-1);
	}

	return(texid);
}

void gdv_resources_toggleMarkTexture()
{
	if(gdv_resources_markIsShowing == 0)
	{
		save_texture_skybox_left = texture_skybox_left;
		save_texture_skybox_front = texture_skybox_front;
		save_texture_skybox_right = texture_skybox_right;
		save_texture_skybox_back = texture_skybox_back;
		save_texture_skybox_top = texture_skybox_top;
		save_texture_skybox_bottom = texture_skybox_bottom;
		save_texture_labels = texture_labels;
		save_texture_field = texture_field;
		save_texture_net = texture_net;
		save_texture_track = texture_track;

		texture_skybox_left = texture_mark;
		texture_skybox_front = texture_mark;
		texture_skybox_right = texture_mark;
		texture_skybox_back = texture_mark;
		texture_skybox_top = texture_mark;
		texture_skybox_bottom = texture_mark;
		texture_labels = texture_mark;
		texture_field = texture_mark;
		texture_net = texture_mark;
		texture_track = texture_mark;

		gdv_resources_markIsShowing = 1;
	}
	else
	{
		texture_skybox_left = save_texture_skybox_left;
		texture_skybox_front = save_texture_skybox_front;
		texture_skybox_right = save_texture_skybox_right;
		texture_skybox_back = save_texture_skybox_back;
		texture_skybox_top = save_texture_skybox_top;
		texture_skybox_bottom = save_texture_skybox_bottom;
		texture_labels = save_texture_labels;
		texture_field = save_texture_field;
		texture_net = save_texture_net;
		texture_track = save_texture_track;

		gdv_resources_markIsShowing = 0;
	}
}
