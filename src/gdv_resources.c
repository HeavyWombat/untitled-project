#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <fakelib.h>

#include <gdv_os.h>

#include <gdv_resources.h>
#include <objecthandler.h>
#include <landscape.h>

GLuint texture_mark;
GLuint texture_labels;

GLuint texture_skybox_left;
GLuint texture_skybox_front;
GLuint texture_skybox_right;
GLuint texture_skybox_back;
GLuint texture_skybox_top;
GLuint texture_skybox_bottom;

GLuint texture_ground;
GLuint texture_optScope;
GLuint texture_reflexScope;

GLuint texture_sign[5];
GLuint texture_tree[2];

GLuint texture_verys;
GLuint texture_sack;
GLuint texture_mat;

GLuint texture_help[3];

GLuint texture_ba;

GLuint texture_block[2];

GLuint texture_text;

GLuint texture_g36sil;

GLuint texture_rainDrop;
GLuint texture_lightning;

static int gdv_resources_markIsShowing = 0;

static GLuint save_texture_labels;

// --- ---

void gdv_resources_loadImages( void )
{
	fake_lib_t *pFakeLib = NULL;
	fake_image_t *pFakeImage = NULL;
	
	pFakeLib = fake_new();
	
	fake_dar_add_dir( pFakeLib, (unsigned char*)"../data" );
	fake_dar_add_dir( pFakeLib, (unsigned char*)"data" );

	pFakeImage = fake_imageFromPNG(pFakeLib, "mark.png");
	fake_image_toRGBA8888(pFakeLib, pFakeImage);
	texture_mark = gdv_resources_textureFromFakeImage(pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0);
	fake_image_delete(pFakeLib, &pFakeImage);

	pFakeImage = fake_imageFromPNG(pFakeLib, "labels.png");
	fake_image_toRGBA8888(pFakeLib, pFakeImage);
	texture_labels = gdv_resources_textureFromFakeImage(pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0);
	fake_image_delete(pFakeLib, &pFakeImage);
  
  // --- ---
  
  heightMap = fake_imageFromPNG( pFakeLib, "heightmap.png" );
  fake_image_toRGBA8888( pFakeLib, heightMap );
  fake_image_delete( pFakeLib, &pFakeImage );
  
  /* sky box */
  pFakeImage = fake_imageFromPNG( pFakeLib, "skybox_back.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_skybox_back = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, 0 );
  fake_image_delete( pFakeLib, &pFakeImage );

  pFakeImage = fake_imageFromPNG( pFakeLib, "skybox_bottom.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_skybox_bottom = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, 0 );
  fake_image_delete( pFakeLib, &pFakeImage );
  
  pFakeImage = fake_imageFromPNG( pFakeLib, "skybox_front.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_skybox_front = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, 0 );
  fake_image_delete( pFakeLib, &pFakeImage );

  pFakeImage = fake_imageFromPNG( pFakeLib, "skybox_left.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_skybox_left = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, 0 );
  fake_image_delete( pFakeLib, &pFakeImage );
  
  pFakeImage = fake_imageFromPNG( pFakeLib, "skybox_right.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_skybox_right = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, 0 );
  fake_image_delete( pFakeLib, &pFakeImage );

  pFakeImage = fake_imageFromPNG( pFakeLib, "skybox_top.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_skybox_top = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, 0 );
  fake_image_delete( pFakeLib, &pFakeImage );
  
  // --- ---
  
  pFakeImage = fake_imageFromPNG( pFakeLib, "ground.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_ground = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, -1 );
  fake_image_delete( pFakeLib, &pFakeImage );
  
  pFakeImage = fake_imageFromPNG( pFakeLib, "veryStoney.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_verys = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, -1 );
  fake_image_delete( pFakeLib, &pFakeImage );
  
  pFakeImage = fake_imageFromPNG( pFakeLib, "sack.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_sack = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, -1 );
  fake_image_delete( pFakeLib, &pFakeImage );
  
  pFakeImage = fake_imageFromPNG( pFakeLib, "mat.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_mat = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, -1 );
  fake_image_delete( pFakeLib, &pFakeImage );

  pFakeImage = fake_imageFromPNG( pFakeLib, "optScope.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_optScope = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0 );
  fake_image_delete( pFakeLib, &pFakeImage );

  pFakeImage = fake_imageFromPNG( pFakeLib, "reflexScope.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_reflexScope = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0 );
  fake_image_delete( pFakeLib, &pFakeImage );
  
  pFakeImage = fake_imageFromPNG( pFakeLib, "tree0.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_tree[0] = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, -1 );
  fake_image_delete( pFakeLib, &pFakeImage );
  
  pFakeImage = fake_imageFromPNG( pFakeLib, "tree1.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_tree[1] = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, -1 );
  fake_image_delete( pFakeLib, &pFakeImage );
  
  pFakeImage = fake_imageFromPNG( pFakeLib, "sign100.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_sign[0] = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, -1 );
  fake_image_delete( pFakeLib, &pFakeImage );

  pFakeImage = fake_imageFromPNG( pFakeLib, "sign150.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_sign[1] = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, -1 );
  fake_image_delete( pFakeLib, &pFakeImage );

  pFakeImage = fake_imageFromPNG( pFakeLib, "sign200.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_sign[2] = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, -1 );
  fake_image_delete( pFakeLib, &pFakeImage );
  
  pFakeImage = fake_imageFromPNG( pFakeLib, "ba.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_ba = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0 );
  fake_image_delete( pFakeLib, &pFakeImage );

  pFakeImage = fake_imageFromPNG( pFakeLib, "front.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_block[0] = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0 );
  fake_image_delete( pFakeLib, &pFakeImage );

  pFakeImage = fake_imageFromPNG( pFakeLib, "back.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_block[1] = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0 );
  fake_image_delete( pFakeLib, &pFakeImage );
  
  pFakeImage = fake_imageFromPNG( pFakeLib, "text.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_text = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0 );
  fake_image_delete( pFakeLib, &pFakeImage );
  
  pFakeImage = fake_imageFromPNG( pFakeLib, "g36.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_g36sil = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0 );
  fake_image_delete( pFakeLib, &pFakeImage );
  
  pFakeImage = fake_imageFromPNG( pFakeLib, "rainDrop.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_rainDrop = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0 );
  fake_image_delete( pFakeLib, &pFakeImage );
  
  pFakeImage = fake_imageFromPNG( pFakeLib, "lightning.png" );
  fake_image_toRGBA8888( pFakeLib, pFakeImage );
  texture_lightning = gdv_resources_textureFromFakeImage( pFakeLib, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0 );
  fake_image_delete( pFakeLib, &pFakeImage );
  

  // ---
  
  glGenTextures( 1, &texture_help[0] );
  glBindTexture( GL_TEXTURE_2D, texture_help[0] );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  
  glGenTextures( 1, &texture_help[1] );
  glBindTexture( GL_TEXTURE_2D, texture_help[1] );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

  glGenTextures( 1, &texture_help[2] );
  glBindTexture( GL_TEXTURE_2D, texture_help[2] );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
}

int gdv_resources_textureFromFakeImage(fake_lib_t *pfl, fake_image_t *fi, GLint minFilter, GLint magFilter, GLint wrapS, GLint wrapT, GLint level)
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
		save_texture_labels = texture_labels;

		texture_labels = texture_mark;

		gdv_resources_markIsShowing = 1;
	}
	else
	{
		texture_labels = save_texture_labels;

		gdv_resources_markIsShowing = 0;
	}
}
