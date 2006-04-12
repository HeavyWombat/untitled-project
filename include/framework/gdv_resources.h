#ifndef __GDV_RESOURCES_H
#define __GDV_RESOURCES_H

#include<fakelib.h>

extern GLuint texture_mark;
extern GLuint texture_labels;

extern GLuint texture_skybox_left;
extern GLuint texture_skybox_front;
extern GLuint texture_skybox_right;
extern GLuint texture_skybox_back;
extern GLuint texture_skybox_top;
extern GLuint texture_skybox_bottom;

extern GLuint texture_ground;
extern GLuint texture_optScope;
extern GLuint texture_reflexScope;

extern GLuint texture_sign[5];
extern GLuint texture_tree[2];

extern GLuint texture_verys;
extern GLuint texture_sack;
extern GLuint texture_mat;

extern GLuint texture_help[3];

extern GLuint texture_ba;

extern GLuint texture_block[2];

extern GLuint texture_text;

extern GLuint texture_g36sil;

extern GLuint texture_rainDrop;
extern GLuint texture_lightning;

void gdv_resources_loadImages( void );
int gdv_resources_textureFromFakeImage(fake_lib_t *pfl, fake_image_t *fi, GLint minFilter, GLint magFilter, GLint wrapS, GLint wrapT, GLint level);
void gdv_resources_toggleMarkTexture();

#endif
