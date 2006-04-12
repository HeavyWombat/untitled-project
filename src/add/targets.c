#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <landscape.h>
#include <targets.h>

// --- ---

static int targetList;

target targets[TARGETS_COUNT];

// --- ---

inline static float silFunc( float f )
{
  return (89.702f * pow( f, 4.0f )) - (118.37f * pow( f, 3.0f )) + (39.201f * pow( f, 2.0f )) - (5.3201f * f) + 2.0494f;
}

static void drawTarget( float x, float y, float z, float arc )
{
  glPushAttrib( GL_ENABLE_BIT );
  
  glDisable( GL_TEXTURE_2D );
  glDisable( GL_LIGHTING );
  
  glPushMatrix();
  
  glTranslatef( x, y, z );
  glRotatef( arc, 0.0f, 0.0f, 1.0f );
  
  // ---
  
  glCallList( targetList );
  
  // ---  
  
  glColor3f( 1.0f, 1.0f, 1.0f );
  
  glPopMatrix();
  
  glPopAttrib();
}

// --- ---

void targets_init( void )
{
  const float depth = 200.0f;
  const float zOffset = -20.0f;
  const float lineDiff = 5.0f;
  const float diff = 3.0f;
  
  float x;
  float y;
  float z;
  
  for( int i=0; i<TARGETS_COUNT; i+=2 )
  {    
    x = depth;
    z = zOffset + (float)i * lineDiff;
    y = getHeightAtPos( x, z );
   
    targets[i].position[0] = x;
    targets[i].position[1] = y;
    targets[i].position[2] = z;
    targets[i].visible = TARGET_MOVING_UP;
    targets[i].arc = -90.0f;
    
    // ---

    x = depth;
    z = zOffset + (float)i * lineDiff + diff;
    y = getHeightAtPos( x, z );
    
    targets[i+1].position[0] = x;
    targets[i+1].position[1] = y;
    targets[i+1].position[2] = z;
    targets[i+1].visible = TARGET_MOVING_UP;
    targets[i+1].arc = -90.0f;
  }
  
  // ---
  
  const float xDep = 0.1f;
  const float zOff = 0.125f;
  const float step = 0.01f;
  
  float p[3];
  
  targetList = glGenLists( 1 );
  glNewList( targetList, GL_COMPILE );
  
  glBegin( GL_TRIANGLE_FAN );
  
  glNormal3f( -1.0f, 0.0f, 0.0f );
  glColor3f( 1.0f, 0.0f, 0.0f );
  
  glVertex3f( 0.0f, 0.0f, 0.0f );
  
  for( float f = 0.5f; f > 0.0f; f -= step )
  {
    p[0] = 0.0f;
    p[1] = silFunc( f );
    p[2] = zOff + f;
    glVertex3fv( p );
  }
  
  glVertex3f( 0.0f, p[1], 0.0f );

  for( float f = 0.0f; f < 0.5f; f += step )
  {
    p[0] = 0.0f;
    p[1] = silFunc( f );
    p[2] = -f - zOff;
    glVertex3fv( p );
  }
  
  glEnd();
  
  glColor3f( 0.0f, 0.0f, 0.0f );
  
    
  glBegin( GL_TRIANGLE_STRIP );
  
  glVertex3f( 0.0f, silFunc( 0.5f ), zOff + 0.5f );
  
  for( float f = 0.5f; f > step; f -= step )
  {
    p[0] = xDep;
    p[1] = silFunc( f );
    p[2] = zOff + f;
    glVertex3fv( p );
    
    p[0] = 0.0f;
    p[1] = silFunc( f - step );
    p[2] = zOff + ( f - step );
    glVertex3fv( p );
  }
  
  glVertex3f( xDep, p[1], p[2] );
  glVertex3f( 0.0f, p[1], -p[2] );
  glVertex3f( xDep, p[1], -p[2] );

  for( float f = step; f < 0.5f; f += step )
  {
    p[0] = 0.0f;
    p[1] = silFunc( f );
    p[2] = -f - zOff;
    glVertex3fv( p );
    
    p[0] = xDep;
    glVertex3fv( p );
  }
  
  glVertex3f( 0.0f, 0.0f, zOff + 0.5f );
  glVertex3f( xDep, 0.0f, zOff + 0.5f );
  
  glEnd();
  
  glColor3f( 1.0f, 0.0f, 0.0f );
  
    
  glBegin( GL_TRIANGLE_FAN );
  
  glNormal3f( 1.0f, 0.0f, 0.0f );
  glColor3f( 1.0f, 0.0f, 0.0f );
  
  glVertex3f( xDep, 0.0f, 0.0f );
  
  for( float f = 0.5f; f > 0.0f; f -= step )
  {
    p[0] = xDep;
    p[1] = silFunc( f );
    p[2] = - (zOff + f);
    glVertex3fv( p );
  }
  
  glVertex3f( xDep, p[1], 0.0f );

  for( float f = 0.0f; f < 0.5f; f += step )
  {
    p[0] = xDep;
    p[1] = silFunc( f );
    p[2] = -( -f - zOff );
    glVertex3fv( p );
  }
  
  glEnd();
  
  glEndList();
}

void targets_draw( double timeDiff )
{
  for( int i = 0; i < TARGETS_COUNT; i++ )
  {
    targets_drawTarget( i );
    
    switch( targets[i].visible )
    {
      case TARGET_VISIBLE:
        break;
      
      case TARGET_INVISIBLE:
        targets[i].timeOut -= timeDiff;
        if( targets[i].timeOut < 0.0 )
          targets[i].visible = TARGET_MOVING_UP;
        break;
        
      case TARGET_MOVING_UP:
        if( targets[i].arc < 0.0f )
        {
          targets[i].arc += 120.0f * timeDiff;
        }
        else
        {
          targets[i].arc = 0.0f;
          targets[i].visible = TARGET_VISIBLE;
        }
        break;
        
      case TARGET_MOVING_DOWN:
        if( targets[i].arc > -90.0f )
        {
          targets[i].arc -= 120.0f * timeDiff;
        }
        else
        {
          targets[i].arc = -90.0f;
          targets[i].visible = TARGET_INVISIBLE;
        }
        break;
      
    }
  }
}

void targets_drawTarget( int number )
{
  drawTarget( targets[number].position[0], targets[number].position[1], targets[number].position[2], targets[number].arc );
}
