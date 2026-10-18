#include <objecthandler.h>
#include <gdv_resources.h>
#include <landscape.h>
#include <objects.h>

static int signsList;
static int groundList;
static int stationList;

static void objects_drawSign( void )
{
  BlenderObject* sign = objecthandler_getObject("Sign");
  if (!sign) return;

  glPushMatrix();

  glScalef( 0.5f, 0.5f, 0.5f );

  glRotatef( -90.0f, 1.0f, 0.0f, 0.0f );

  glRotatef( 90.0f, 0.0f, 0.0f, 1.0f );

  objecthandler_drawObject( sign );

  glPopMatrix();
}

static void objects_drawSack( void )
{
  BlenderObject* sack = objecthandler_getObject("Sack");
  if (!sack) return;

  glPushMatrix();

  // glScalef( 0.5f, 0.5f, 0.5f );

  glRotatef( -90.0f, 1.0f, 0.0f, 0.0f );
  glRotatef( 90.0f, 0.0f, 0.0f, 1.0f );

  objecthandler_drawObject( sack );

  glPopMatrix();
}

// --- ---

void objects_init( void )
{
  BlenderObject* sign = objecthandler_getObject("Sign");
  BlenderObject* ground = objecthandler_getObject("Ground");
  BlenderObject* sack = objecthandler_getObject("Sack");

  if (sign) {
    objecthandler_setTexture(sign, texture_sign[0]);
  }
  if (ground) {
    objecthandler_setTexture(ground, texture_verys);
  }
  if (sack) {
    objecthandler_setTexture(sack, texture_sack);
  }

  // ---

  float point[3];

  signsList = glGenLists( 1 );
  glNewList( signsList, GL_COMPILE );

  sign = objecthandler_getObject("Sign");
  if (sign) {
    for( int i = 0; i<3; i++ )
    {
      point[0] = 100.0f + (float)i * 50.0f;
      point[2] = 25.0f;
      point[1] = getHeightAtPos( point[0], point[2] );

      objecthandler_setTexture(sign, texture_sign[i]);

      glPushMatrix();
      glTranslatef( point[0], point[1], point[2] );
      objects_drawSign();
      glPopMatrix();
    }
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

  ground = objecthandler_getObject("Ground");
  if (ground) {
    glPushMatrix();

      glTranslatef( -5.0f, -0.2f, -20.0f );

      glPushMatrix();
        glRotatef( -90.0f, 1.0f, 0.0f, 0.0f );
        glRotatef( -90.0f, 0.0f, 0.0f, 1.0f );
        objecthandler_drawObject( ground );
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
  }

  glEndList();
}

void objects_draw( void )
{
  glCallList( signsList );

  glCallList( groundList );
}
