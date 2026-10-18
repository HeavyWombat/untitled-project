#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gl_compat.h>

#include <fakelib.h>
#include <image_loader.h>

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
	fake_image_t *pFakeImage = NULL;
	
	/* Direct PNG loading without archive system */
	printf("\n=== Loading Textures ===\n");

	pFakeImage = load_png_image("mark.png");
	texture_mark = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0);
	free_image(pFakeImage);

	pFakeImage = load_png_image("labels.png");
	texture_labels = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0);
	free_image(pFakeImage);

  // --- ---

  heightMap = load_png_image("heightmap.png");
  if (!heightMap) {
      fprintf(stderr, "ERROR: Failed to load heightmap, terrain will not work properly\n");
      /* Create a minimal fallback heightmap */
      heightMap = load_png_image("fallback_that_does_not_exist.png");  /* Will create checkerboard */
  }

  /* sky box */
  pFakeImage = load_png_image("skybox_back.png");
  texture_skybox_back = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, 0);
  free_image(pFakeImage);

  pFakeImage = load_png_image("skybox_bottom.png");
  texture_skybox_bottom = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, 0);
  free_image(pFakeImage);

  pFakeImage = load_png_image("skybox_front.png");
  texture_skybox_front = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, 0);
  free_image(pFakeImage);

  pFakeImage = load_png_image("skybox_left.png");
  texture_skybox_left = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, 0);
  free_image(pFakeImage);

  pFakeImage = load_png_image("skybox_right.png");
  texture_skybox_right = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, 0);
  free_image(pFakeImage);

  pFakeImage = load_png_image("skybox_top.png");
  texture_skybox_top = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, 0);
  free_image(pFakeImage);

  // --- ---

  pFakeImage = load_png_image("ground.png");
  texture_ground = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, -1);
  free_image(pFakeImage);

  pFakeImage = load_png_image("veryStoney.png");
  texture_verys = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, -1);
  free_image(pFakeImage);

  pFakeImage = load_png_image("sack.png");
  texture_sack = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, -1);
  free_image(pFakeImage);

  pFakeImage = load_png_image("mat.png");
  texture_mat = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, -1);
  free_image(pFakeImage);

  pFakeImage = load_png_image("optScope.png");
  texture_optScope = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0);
  free_image(pFakeImage);

  pFakeImage = load_png_image("reflexScope.png");
  texture_reflexScope = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0);
  free_image(pFakeImage);

  pFakeImage = load_png_image("tree0.png");
  texture_tree[0] = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, -1);
  free_image(pFakeImage);

  pFakeImage = load_png_image("tree1.png");
  texture_tree[1] = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, -1);
  free_image(pFakeImage);

  pFakeImage = load_png_image("sign100.png");
  texture_sign[0] = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, -1);
  free_image(pFakeImage);

  pFakeImage = load_png_image("sign150.png");
  texture_sign[1] = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, -1);
  free_image(pFakeImage);

  pFakeImage = load_png_image("sign200.png");
  texture_sign[2] = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, -1);
  free_image(pFakeImage);

  pFakeImage = load_png_image("ba.png");
  texture_ba = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0);
  free_image(pFakeImage);

  pFakeImage = load_png_image("front.png");
  texture_block[0] = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0);
  free_image(pFakeImage);

  pFakeImage = load_png_image("back.png");
  texture_block[1] = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0);
  free_image(pFakeImage);

  pFakeImage = load_png_image("text.png");
  texture_text = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0);
  free_image(pFakeImage);

  pFakeImage = load_png_image("g36.png");
  texture_g36sil = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0);
  free_image(pFakeImage);

  pFakeImage = load_png_image("rainDrop.png");
  texture_rainDrop = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0);
  free_image(pFakeImage);

  pFakeImage = load_png_image("lightning.png");
  texture_lightning = gdv_resources_textureFromFakeImage(NULL, pFakeImage, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0);
  free_image(pFakeImage);


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

  printf("=== Texture Loading Complete ===\n\n");
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
