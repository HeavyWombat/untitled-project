#include <GL/gl.h>
#include <GL/glu.h>

#include <targets.h>
#include <selection.h>

// --- ---

// --- ---

// --- ---

void selection_select( int mouseX, int mouseY )
{
  GLint hits;
  GLuint buffer[512];
  GLint viewport[4];

  for( int i=0; i<TARGETS_COUNT; i++ )
  {
    glGetIntegerv( GL_VIEWPORT, viewport );
  
    glSelectBuffer( 512, buffer );
  
    glRenderMode( GL_SELECT );
  
    glInitNames();
    glPushName( 0 );
  
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix( (GLdouble)mouseX, (GLdouble)(viewport[3] - mouseY), 1.0f, 1.0f, viewport );
    gluPerspective( 45.0f, (GLfloat)(viewport[2] - viewport[0]) / (GLfloat)(viewport[3] - viewport[1]), 0.1f, 300.0f );
    glMatrixMode( GL_MODELVIEW );
    
    glLoadName( 0 );
    
    // ---
    
    targets_drawTarget( i );
    
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
    
    // ---
    
    hits = glRenderMode( GL_RENDER );
  
    if( hits > 0 )
    {
      targets[i].visible = TARGET_MOVING_DOWN;
      targets[i].timeOut = TARGET_DEFAULT_TIMEOUT;
    }
  }
}
