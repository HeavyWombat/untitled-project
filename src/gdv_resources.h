#ifndef __GDV_RESOURCES_H
#define __GDV_RESOURCES_H

#include<fakelib.h>

extern GLuint texture_mark;
extern GLuint texture_skybox_left;
extern GLuint texture_skybox_front;
extern GLuint texture_skybox_right;
extern GLuint texture_skybox_back;
extern GLuint texture_skybox_top;
extern GLuint texture_skybox_bottom;
extern GLuint texture_labels;
extern GLuint texture_field;
extern GLuint texture_net;
extern GLuint texture_track;

int gdv_resources_loadImages();
GLuint gdv_resources_textureFromFakeImage(fake_lib_t *pfl, fake_image_t *fi, GLint minFilter, GLint magFilter, GLint wrapS, GLint wrapT, GLint level);
void gdv_resources_toggleMarkTexture();

#endif

