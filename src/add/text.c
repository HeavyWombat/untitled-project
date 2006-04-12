#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <gdv_os.h>
#include <display.h>
#include <text.h>

// --- ---

static float defaultTop = 10.0f;
static float top = defaultTop;

static float leftOffset = -204.8f;
static float topOffset = 192.0f;

static float defaultLeft = 0.0f;

// --- ---

// --- ---

void text_init( void )
{
  text_setDefaults();
}

void text_setDefaults( void )
{
  text_setDefaultTop( 10.0f );
  text_setDefaultLeft( 0.0f );
  
  text_setLeftOffset( -0.20f );
  text_setTopOffset( 0.25f );
}

void text_setDefaultTop( float f )
{
  defaultTop = f;
}

void text_setDefaultLeft( float f )
{
  defaultLeft = f;
}

void text_setLeftOffset( float f )
{
  leftOffset = f * (float)display_width;
}

void text_setTopOffset( float f )
{
  topOffset = f * (float)display_height;
}

void text_drawText( int left, char* text )
{
  const float leftShift = defaultLeft + (float)left * 8.0f;
  
  glPushAttrib( GL_ENABLE_BIT );
  glDisable( GL_TEXTURE_2D );
  glDisable( GL_LIGHTING );
  glDisable( GL_BLEND );
  
  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D( -0.5f, 0.5f, -0.5f, 0.5f );
  
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();

  glRasterPos2f( 0.0f, 0.0f );
  glBitmap( 0, 0, 0, 0, leftOffset + leftShift, topOffset - top, NULL );

  // ---
  
  gdv_os_drawBitmappedText( text );
  
  // ---
               
  glMatrixMode( GL_PROJECTION );
  glPopMatrix();

  glMatrixMode( GL_MODELVIEW );
  glPopMatrix();
  
  glPopAttrib();
}

void text_drawTextLn( int left, char* text )
{
  text_drawText( left, text );
  text_Ln();
}

void text_Ln( void )
{
  const float step = 12.0f;
  top += step;
}

void text_nLn( unsigned int n )
{
  for( unsigned int i=0; i<n; i++ )
    text_Ln();
}

void text_resetTextOffset( void )
{
  top = defaultTop;
}
