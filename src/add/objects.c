#include <objecthandler.h>
#include <gdv_resources.h>
#include <landscape.h>
#include <objects.h>

static int signsList;
static int groundList;
static int stationList;

static void objects_drawSign( void )
{
  glPushMatrix();
  
  glScalef( 0.5f, 0.5f, 0.5f );
  
  glRotatef( -90.0f, 1.0f, 0.0f, 0.0f );
  
  glRotatef( 90.0f, 0.0f, 0.0f, 1.0f );
  
  objecthandler_drawObject( objecthandler_objects["Sign"] );

  glPopMatrix();  
}

static void objects_drawSack( void )
{
  glPushMatrix();
  
  // glScalef( 0.5f, 0.5f, 0.5f );
  
  glRotatef( -90.0f, 1.0f, 0.0f, 0.0f );
  glRotatef( 90.0f, 0.0f, 0.0f, 1.0f );
  
  objecthandler_drawObject( objecthandler_objects["Sack"] );

  glPopMatrix();  
}

// --- ---

void objects_init( void )
{
  objecthandler_objects["Sign"]->useTexture = 1;
  objecthandler_objects["Sign"]->textureID = texture_sign[0];
  
  objecthandler_objects["Ground"]->useTexture = 1;
  objecthandler_objects["Ground"]->textureID = texture_verys;
  
  objecthandler_objects["Sack"]->useTexture = 1;
  objecthandler_objects["Sack"]->textureID = texture_sack;
  
  // ---
  
  float point[3];
  
  signsList = glGenLists( 1 );
  glNewList( signsList, GL_COMPILE );
  
  for( int i = 0; i<3; i++ )
  {
    point[0] = 100.0f + (float)i * 50.0f;
    point[2] = 25.0f;
    point[1] = getHeightAtPos( point[0], point[2] );
    
    objecthandler_objects["Sign"]->textureID = texture_sign[i];
    
    glPushMatrix();
    glTranslatef( point[0], point[1], point[2] );
    objects_drawSign();
    glPopMatrix();
  }
  
  glEndList();
  
  // ---
  
  stationList = glGenLists( 1 );
  glNewList( stationList, GL_COMPILE );
  
  glPushMatrix();
  
  glPushAttrib( GL_ENABLE_BIT );
  glEnable( GL_TEXTURE_2D );
  glEnable( GL_BLEND );
  glBindTexture( GL_TEXTURE_2D, texture_mat );
  glBegin( GL_QUADS );
  glNormal3f( 0.0f, 1.0f, 0.0f );
  glTexCoord2d( 0, 1 );
  glVertex3f( 0.0f, 0.0f, 0.0f );
  glTexCoord2d( 1, 1 );
  glVertex3f( 0.0f, 0.0f, 1.0f );
  glTexCoord2d( 1, 0 );
  glVertex3f( 2.0f, 0.0f, 1.0f );
  glTexCoord2d( 0, 0 );
  glVertex3f( 2.0f, 0.0f, 0.0f );
  glEnd();
  glPopAttrib();
  
  glTranslatef( 1.85f, 0.04f, 0.35f );
  objects_drawSack();
  
  glTranslatef( 0.0f, 0.0f, 0.35f );
  objects_drawSack();
  
  glTranslatef( 0.0f, 0.075f, -0.35f );
  objects_drawSack();
  
  glPopMatrix(); 
  
  glEndList();
  
  // ---
  
  groundList = glGenLists( 1 );
  glNewList( groundList, GL_COMPILE );
  
  glPushMatrix();

    glTranslatef( -5.0f, -0.2f, -20.0f );

    glPushMatrix();
      glRotatef( -90.0f, 1.0f, 0.0f, 0.0f );
      glRotatef( -90.0f, 0.0f, 0.0f, 1.0f );
      objecthandler_drawObject( objecthandler_objects["Ground"] );
    glPopMatrix();
  
    glPushMatrix();
    glTranslatef( 3.0f, 0.1f, 0.0f );
    for( int i=0; i<4; i++ )
    {
      glTranslatef( 0.0f, 0.0f, 6.0f );
      glCallList( stationList );
    }
    glPopMatrix();
  
  glPopMatrix(); 
  
  glEndList();
}

void objects_draw( void )
{
  glCallList( signsList );
  
  glCallList( groundList );
}
