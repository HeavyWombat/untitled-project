#include <GL/gl.h>
#include <GL/glu.h>

#include <gdv_os.h>
#include <gdv_main.h>
#include <gdv_resources.h>
#include <display.h>
#include <cameraMovement.h>
#include <text.h>
#include <ttext.h>
#include <splash.h>

// --- ---

static int cubeList;
static int blockList;
static float angleY = 0.0f;
static float offsetX = 0.0f;
static float zoom = 1.1f;
static const double visible = 5.0;

// --- ---

// --- ---

void splash_init( void )
{
  cubeList = glGenLists( 1 );
  glNewList( cubeList, GL_COMPILE );
  
  glBindTexture( GL_TEXTURE_2D, texture_ba );
  
  glBegin( GL_QUADS );
  
  // front
  glNormal3f( -1.0f, 0.0f, 0.0f );
  glTexCoord2i( 0, 0 );
  glVertex3f( -0.5f, +0.5f, -0.5f );
  
  glTexCoord2i( 0, 1 );
  glVertex3f( -0.5f, -0.5f, -0.5f );
  
  glTexCoord2i( 1, 1 );
  glVertex3f( -0.5f, -0.5f, +0.5f );
  
  glTexCoord2i( 1, 0 );
  glVertex3f( -0.5f, +0.5f, +0.5f );
  
  // back
  glNormal3f( +1.0f, 0.0f, 0.0f );
  glTexCoord2i( 0, 0 );
  glVertex3f( +0.5f, +0.5f, +0.5f );
  
  glTexCoord2i( 0, 1 );
  glVertex3f( +0.5f, -0.5f, +0.5f );
  
  glTexCoord2i( 1, 1 );
  glVertex3f( +0.5f, -0.5f, -0.5f );
  
  glTexCoord2i( 1, 0 );
  glVertex3f( +0.5f, +0.5f, -0.5f );
  
  // right
  glNormal3f( 0.0f, 0.0f, +1.0f );
  glTexCoord2i( 0, 0 );
  glVertex3f( -0.5f, +0.5f, +0.5f );
  
  glTexCoord2i( 0, 1 );
  glVertex3f( -0.5f, -0.5f, +0.5f );
  
  glTexCoord2i( 1, 1 );
  glVertex3f( +0.5f, -0.5f, +0.5f );
  
  glTexCoord2i( 1, 0 );
  glVertex3f( +0.5f, +0.5f, +0.5f );
  
  // left
  glNormal3f( 0.0f, 0.0f, -1.0f );
  glTexCoord2i( 0, 0 );
  glVertex3f( +0.5f, +0.5f, -0.5f );
  
  glTexCoord2i( 0, 1 );
  glVertex3f( +0.5f, -0.5f, -0.5f );
  
  glTexCoord2i( 1, 1 );
  glVertex3f( -0.5f, -0.5f, -0.5f );
  
  glTexCoord2i( 1, 0 );
  glVertex3f( -0.5f, +0.5f, -0.5f );
  
  // top
  glNormal3f( 0.0f, +1.0f, 0.0f );
  glTexCoord2i( 0, 0 );
  glVertex3f( +0.5f, +0.5f, -0.5f );
  
  glTexCoord2i( 0, 1 );
  glVertex3f( -0.5f, +0.5f, -0.5f );
  
  glTexCoord2i( 1, 1 );
  glVertex3f( -0.5f, +0.5f, +0.5f );
  
  glTexCoord2i( 1, 0 );
  glVertex3f( +0.5f, +0.5f, +0.5f );

  // bottom
  glNormal3f( 0.0f, -1.0f, 0.0f );
  glTexCoord2i( 0, 0 );
  glVertex3f( -0.5f, -0.5f, -0.5f );
  
  glTexCoord2i( 0, 1 );
  glVertex3f( +0.5f, -0.5f, -0.5f );
  
  glTexCoord2i( 1, 1 );
  glVertex3f( +0.5f, -0.5f, +0.5f );
  
  glTexCoord2i( 1, 0 );
  glVertex3f( -0.5f, -0.5f, +0.5f );
  
  glEnd();
  
  glEndList();
  
  // ---
  
  blockList = glGenLists( 1 );
  glNewList( blockList, GL_COMPILE );
  
  glBindTexture( GL_TEXTURE_2D, texture_block[0] );
  glBegin( GL_QUADS );
  
  glNormal3f( -1.0f, 0.0f, 0.0f );
  glTexCoord2i( 0, 0 );
  glVertex3f( 0.0f, +0.5f, -0.5f );
  
  glTexCoord2i( 0, 1 );
  glVertex3f( 0.0f, -0.5f, -0.5f );
  
  glTexCoord2i( 1, 1 );
  glVertex3f( 0.0f, -0.5f, +0.5f );
  
  glTexCoord2i( 1, 0 );
  glVertex3f( 0.0f, +0.5f, +0.5f );
  
  glEnd();
  
  glBindTexture( GL_TEXTURE_2D, texture_block[1] );
  glBegin( GL_QUADS );

  glNormal3f( +1.0f, 0.0f, 0.0f );
  glTexCoord2i( 0, 0 );
  glVertex3f( 0.0f, +0.5f, +0.5f );
  
  glTexCoord2i( 0, 1 );
  glVertex3f( 0.0f, -0.5f, +0.5f );
  
  glTexCoord2i( 1, 1 );
  glVertex3f( 0.0f, -0.5f, -0.5f );
  
  glTexCoord2i( 1, 0 );
  glVertex3f( 0.0f, +0.5f, -0.5f );
  
  glEnd();
  
  glEndList();
}

void splash_draw( double timeDiff )
{
  const float angleFactor = 90.0f;
  const float factor = 0.5f;
  
  static double counter = 0.0;
  counter += timeDiff;
  
  glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glViewport( 0, 0, display_width, display_height );

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  
  gluPerspective( defaultFOV, aspectRatio, minDist, maxDist );
  
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  gluLookAt( 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f );

  glColor3f( 1.0f, 1.0f, 1.0f );

  switch( (int)(counter/visible) )
  {
    case 0:
      text_setLeftOffset( -0.45f );
    
      text_drawTextLn( 0, GDV_PROJECT_NAME );
      text_nLn( 4 );
      
      text_drawTextLn( 0, "Dieses Programm wurde im Rahmen des Faches Graphische Datenverarbeitung" );
      text_drawTextLn( 0, "an der Berufsakademie Stuttgart von Markus Blümchen und Matthias Diester" );
      text_drawTextLn( 0, "entwickelt." );
      text_Ln();
      
      text_drawTextLn( 0, "Folgende Anforderungen sollten eingearbeitet werden:" );
      text_drawTextLn( 0, "Die Demo oder das Spiel soll eine kurze Story enthalten oder erzählen." );
      text_drawTextLn( 0, "1 Objekt, erzeugt mit Blender (Blender File muss mitabgegeben werden)" );
      text_drawTextLn( 0, "1 Objekt, erzeugt im Code" );
      text_drawTextLn( 0, "1 texturiertes Objekt" );
      text_drawTextLn( 0, "1 Lichtquelle (korrekt beleuchtete Objekte)" );
      text_drawTextLn( 0, "1 Effekt (optisch, geometrisch, mathematisch, ...)" );
      text_drawTextLn( 0, "1 Animation (bewegte Objekte und/oder bewegte Kamera, ...)" );
      text_drawTextLn( 0, "Die Animationen / Bewegungen müssen von der Zeit abhängen." );
      text_drawTextLn( 0, "Abbruch jederzeit per ESCape" );
      text_drawTextLn( 0, "Es muss das gegebene Framework als Basis verwendet werden." );
      text_drawTextLn( 0, "Sollte Plattform unabhängig programmiert sein." );
      text_drawTextLn( 0, "Muss in irgendeiner Form das BA Logo enthalten." );
      text_drawTextLn( 0, "Die Namen der Beteiligten müssen in irgendeiner Form enthalten sein (Vorspann/Abspann) (Fotos können auch eingebunden werden)." );
      text_drawTextLn( 0, "Wenn Sound eingebaut sein soll nur mit fmod (liegt dem Framework bei)." );
      text_drawTextLn( 0, "Urheberrecht bei allem beachten!" );
      text_drawTextLn( 0, "Demo: Demo bzw. Animation muss Endlos laufen." );
      text_drawTextLn( 0, "Spiel: Bei drücken von F1 muss die Tastaturbelegung (Steuerung) angezeigt werden und eventuell eine kurze Spielerklärung." );
      
      text_resetTextOffset();
      break;
    
    case 1:
      text_setLeftOffset( -0.45f );
    
      text_drawTextLn( 0, GDV_PROJECT_NAME );
      text_nLn( 4 );
      
      text_drawTextLn( 0, "Ein BA-Logo ..." );
      text_Ln();
      
      text_resetTextOffset();
      
      glPushAttrib( GL_ENABLE_BIT );
      glEnable( GL_LIGHTING );
      glEnable( GL_LIGHT0 );
      glEnable( GL_TEXTURE_2D );
      
      glPushMatrix();

      glTranslatef( 4.0f, 0.0f, 0.0f );
      glRotatef( 25.0f * counter, 1.0f, 0.0f, 0.0f );
      glRotatef( 50.0f * counter, 0.0f, 1.0f, 0.0f );
      glRotatef( 75.0f * counter, 0.0f, 0.0f, 1.0f );
      
      glCallList( cubeList );
      
      glPopMatrix();
      
      glPopAttrib();
      break;
    
    default:
      text_setLeftOffset( -0.45f );
    
      text_drawTextLn( 0, GDV_PROJECT_NAME );
      text_nLn( 4 );
      
      text_drawTextLn( 0, "rechts drehen" );
      text_drawTextLn( 0, "... rechte Cursortaste" );
      text_Ln();

      text_drawTextLn( 0, "links drehen" );
      text_drawTextLn( 0, "... links Cursortaste" );
      text_Ln();
      
      text_drawTextLn( 0, "nach oben schieben" ); 
      text_drawTextLn( 0, "... obere Cursortaste" );
      text_Ln();
      
      text_drawTextLn( 0, "nach unten schieben" );
      text_drawTextLn( 0, "... untere Cursortaste" );
      text_Ln();
      
      text_drawTextLn( 0, "heran zoomen" );
      text_drawTextLn( 0, "... POS1 Taste" );
      text_Ln();
      
      text_drawTextLn( 0, "weg zoomen" );
      text_drawTextLn( 0, "... Ende Taste" );
      text_Ln();
      
      text_drawTextLn( 0, "weiter mit Leertaste" );
      text_Ln();

      text_resetTextOffset();
    
      glPushAttrib( GL_ENABLE_BIT );
  
      glEnable( GL_LIGHT0 );
      glEnable( GL_TEXTURE_2D );
      glEnable( GL_BLEND );
      
      glPushMatrix();
    
      glTranslatef( zoom, offsetX, 0.0f );
      glRotatef( angleY, 0.0f, 1.0f, 0.0f );
      
      glCallList( blockList );
      
      glPopMatrix();
      glPopAttrib();
      
      // ---
      
      if( gdv_key_states[ GDV_KEY_RIGHT ] )
        angleY -= angleFactor * timeDiff;
      
      if( gdv_key_states[ GDV_KEY_LEFT ] )
        angleY += angleFactor * timeDiff;
        
      if( gdv_key_states[ GDV_KEY_UP ] )
        offsetX += factor * timeDiff;
      
      if( gdv_key_states[ GDV_KEY_DOWN ] )
        offsetX -= factor * timeDiff;
    
      if( gdv_key_states[ GDV_KEY_HOME ] )
        zoom -= factor * timeDiff;
      
      if( gdv_key_states[ GDV_KEY_END ] )
        zoom += factor * timeDiff;
      
      break;
  }
    
  glFlush();

  if( gdv_key_states[ (int)' ' ] )
    gameMode = GAME_MODE_DRAW;
}
