#include <GL/gl.h>
#include <GL/glu.h>

#include <gdv_resources.h>
#include <cameraMovement.h>
#include <ttext.h>
#include <hud.h>

void hud_init( void )
{
}

void hud_draw( double timeDiff )
{
  glPushAttrib( GL_ENABLE_BIT );
  
  glDisable( GL_LIGHTING );
  glDisable( GL_DEPTH_TEST );
  
  glEnable( GL_TEXTURE_2D );
  glEnable( GL_BLEND );
  
  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();
  
  // ---
  
  const float width = 0.5f;
  const float height = width * 0.5f;
  const float yP = 1.0f;
  const float zP = -1.0f;
  
  glBindTexture( GL_TEXTURE_2D, texture_g36sil );
  
  glBegin( GL_QUADS );
  
  glTexCoord2i( 0, 1 );
  glVertex2f( yP - width, zP );
  
  glTexCoord2i( 1, 1 );
  glVertex2f( yP, zP );
  
  glTexCoord2i( 1, 0 );
  glVertex2f( yP, zP + height );
  
  glTexCoord2i( 0, 0 );
  glVertex2f( yP - width, zP + height );
  
  glEnd();
  
  switch( cameraMode )
  {
    case CAMERA_MODE_WALK:
      ttext_drawText( 0.0f, 1.0e10f, "freier Blick" );
      break;

    case CAMERA_MODE_REFLEX:
      ttext_drawText( 0.0f, 1.0e10f, "Reflexvisier" );
      break;

    case CAMERA_MODE_OPT:
      ttext_drawText( 0.0f, 1.0e10f, "3-fach optisches Visier" );
      break;
  }
  
  // ---    

  glPopMatrix();
  glPopMatrix();
  glPopAttrib();
}
