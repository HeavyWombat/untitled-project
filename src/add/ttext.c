#include <stdio.h>
#include <string.h>
#include <math.h>

#include <gl_compat.h>

#include <gdv_resources.h>
#include <ttext.h>

// --- ---

static float height = 0.075f;
static float width;  /* Calculated in init */
static float narrowValue;  /* Calculated in init */
static GLuint ttext[256];

// --- ---

/* The font texture is indexed by single byte values (CP1252/Latin-1 style
   layout), while the strings in the source files are UTF-8. Map a Unicode
   code point to the matching atlas slot. */
static unsigned char ttext_codepointToAtlasIndex( unsigned int codepoint )
{
  if( codepoint < 0x80 || ( codepoint >= 0xA0 && codepoint <= 0xFF ) )
    return (unsigned char)codepoint;

  switch( codepoint )
  {
    case 0x20AC: return 0x80;  /* Euro sign */
    case 0x201A: return 0x82;
    case 0x0192: return 0x83;
    case 0x201E: return 0x84;
    case 0x2026: return 0x85;
    case 0x2020: return 0x86;
    case 0x2021: return 0x87;
    case 0x02C6: return 0x88;
    case 0x2030: return 0x89;
    case 0x0160: return 0x8A;
    case 0x2039: return 0x8B;
    case 0x0152: return 0x8C;
    case 0x017D: return 0x8E;
    case 0x2018: return 0x91;
    case 0x2019: return 0x92;
    case 0x201C: return 0x93;
    case 0x201D: return 0x94;
    case 0x2022: return 0x95;
    case 0x2013: return 0x96;
    case 0x2014: return 0x97;
    case 0x02DC: return 0x98;
    case 0x2122: return 0x99;
    case 0x0161: return 0x9A;
    case 0x203A: return 0x9B;
    case 0x0153: return 0x9C;
    case 0x017E: return 0x9E;
    case 0x0178: return 0x9F;
    default: return '?';
  }
}

/* Convert a UTF-8 string to atlas indices. Invalid sequences are treated as
   raw Latin-1 bytes. Returns the number of characters written. */
static int ttext_convertText( char* buffer, int capacity, const char* text )
{
  const unsigned char* src = (const unsigned char*)text;
  int used = 0;

  while( ( *src != '\0' ) && ( used < capacity - 1 ) )
  {
    unsigned int codepoint;

    if( *src < 0x80 )
    {
      codepoint = *src;
      src++;
    }
    else if( ( ( *src & 0xE0 ) == 0xC0 ) && ( src[1] != '\0' ) &&
             ( ( src[1] & 0xC0 ) == 0x80 ) )
    {
      codepoint = ( ( *src & 0x1F ) << 6 ) | ( src[1] & 0x3F );
      src += 2;

      if( codepoint < 0x80 )
        codepoint = '?';
    }
    else if( ( ( *src & 0xF0 ) == 0xE0 ) && ( src[1] != '\0' ) && ( src[2] != '\0' ) &&
             ( ( src[1] & 0xC0 ) == 0x80 ) && ( ( src[2] & 0xC0 ) == 0x80 ) )
    {
      codepoint = ( ( *src & 0x0F ) << 12 ) | ( ( src[1] & 0x3F ) << 6 ) | ( src[2] & 0x3F );
      src += 3;

      if( codepoint < 0x800 )
        codepoint = '?';
    }
    else
    {
      codepoint = *src;
      src++;
    }

    buffer[used] = (char)ttext_codepointToAtlasIndex( codepoint );
    used++;
  }

  buffer[used] = '\0';

  return used;
}

// --- ---

// --- ---

void ttext_setHeight( float h )
{
  height = h;
  width = height / sqrt( 2.0f );
  narrowValue = height * 0.3f;

  glDeleteLists( ttext[0], 256 );
  ttext_init();
}

void ttext_init( void )
{
  /* Initialize runtime-calculated values */
  width = height / sqrt( 2.0f );
  narrowValue = height * 0.3f;

  int idx = 0;

  ttext[0] = glGenLists( 256 );

  float yP = 0.0f;
  float zP = 0.0f;

  float leftOffset = 0.0f;
  float topOffset = 0.0f;
  float cWidth = 32.0f / 512.0f;
  float cHeight = 32.0f / 512.0f;

  for( int i=0; i<16; i++ )
  {
    for( int j=0; j<16; j++ )
    {
      leftOffset = (float)j * cWidth;
      topOffset = (float)i * cHeight;

      ttext[idx] = ttext[0] + idx;

      glNewList( ttext[idx], GL_COMPILE );

      glBegin( GL_QUADS );
      glTexCoord2f( leftOffset, topOffset + cHeight );
      glVertex2f( yP, zP );

      glTexCoord2f( leftOffset + cWidth, topOffset + cHeight );
      glVertex2f( yP + width, zP );

      glTexCoord2f( leftOffset + cWidth, topOffset );
      glVertex2f( yP + width, zP + height );

      glTexCoord2f( leftOffset, topOffset );
      glVertex2f( yP, zP + height );
      glEnd();

      glTranslatef( width - narrowValue, 0.0f, 0.0f );

      glEndList();

      idx++;
    }
  }
}

void ttext_drawText( float y, float z, char* text )
{
  static char convertedText[256];
  int length = ttext_convertText( convertedText, (int)sizeof( convertedText ), text );

  y = ( y > 1.0f )? 1.0f - ( length + 1 ) * ( width - narrowValue ): 2.0f * y - 1.0f;
  z = ( z > 1.0f )? -1.0f: ( -2.0f * z + 1.0f) - height;

  glPushAttrib( GL_ENABLE_BIT );

  glDisable( GL_DEPTH_TEST );
  glDisable( GL_LIGHTING );

  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();

  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();

  // ---

  glTranslatef( y, z, 0.0f );

  glEnable( GL_TEXTURE_2D );
  glEnable( GL_BLEND );

  glBindTexture( GL_TEXTURE_2D, texture_text );

  glListBase( ttext[0] );
  glCallLists( length, GL_UNSIGNED_BYTE, convertedText );

  // ---

  glPopMatrix();
  glPopMatrix();
  glPopAttrib();
}
