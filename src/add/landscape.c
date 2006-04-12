#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <display.h>
#include <landscape.h>
#include <gdv_resources.h>

#include <additionalMath.h>

// --- ---

fake_image_t* heightMap;

float* heightMapPoints;

static int landscapeList;

// --- ---

// --- ---

void landscape_init()
{
  const int maxRow = heightMap->height - 1;
  const int maxCol = heightMap->width - 1;
  const int arraySize = ( maxCol * maxRow ) << 1;

  int idx = 0;

	float p0[4];
  float p1[4];
  float p2[4];
  float p3[4];
  float n[3];
  
  faceWithNormal landscape[ arraySize ]; 

  // --- ---

  for( int row = 0; row < maxRow; row++ )
		for( int col = 0; col < maxCol; col++ )
		{
			p0[0] = ((float)col) * LANDSCAPE_FACTOR - X_OFFSET;
			p1[0] = ((float)col) * LANDSCAPE_FACTOR - X_OFFSET;
			p2[0] = ((float)col+1) * LANDSCAPE_FACTOR - X_OFFSET;
			p3[0] = ((float)col+1) * LANDSCAPE_FACTOR - X_OFFSET;
			
			p0[1] = (heightMap->data[ (row * heightMap->width + col) * 4 ]) * LANDSCAPE_MAX_HEIGHT / 255.0;
			p1[1] = (heightMap->data[ ((row+1) * heightMap->width + col) * 4 ]) * LANDSCAPE_MAX_HEIGHT / 255.0;
			p2[1] = (heightMap->data[ (row * heightMap->width + (col+1)) * 4 ]) * LANDSCAPE_MAX_HEIGHT / 255.0;
			p3[1] = (heightMap->data[ ((row+1) * heightMap->width + (col+1)) * 4 ]) * LANDSCAPE_MAX_HEIGHT / 255.0;
			
			p0[2] = ((float)row) * LANDSCAPE_FACTOR - Z_OFFSET;
			p1[2] = ((float)row+1) * LANDSCAPE_FACTOR - Z_OFFSET;
			p2[2] = ((float)row) * LANDSCAPE_FACTOR - Z_OFFSET;
			p3[2] = ((float)row+1) * LANDSCAPE_FACTOR - Z_OFFSET;

      p0[3] = heightMap->data[ (row * heightMap->width + col) * 4 + 3];
      p1[3] = heightMap->data[ ((row+1) * heightMap->width + col) * 4 + 3];
      p2[3] = heightMap->data[ (row * heightMap->width + (col+1)) * 4 + 3];
      p3[3] = heightMap->data[ ((row+1) * heightMap->width + (col+1)) * 4 + 3];
      
      // ---
      
      copyVec( p0, landscape[idx].p0 );
      copyVec( p3, landscape[idx].p1 );
      copyVec( p2, landscape[idx].p2 );

      faceNormal( p0, p3, p2, n );
      
      copyVec( n, landscape[idx].n0 );
      copyVec( n, landscape[idx].n1 );
      copyVec( n, landscape[idx].n2 );
      copyVec( n, landscape[idx].n );
      
      if( equals( p0[3], 0.0f ) && equals( p3[3], 0.0f ) && equals( p2[3], 0.0f ) )
        landscape[idx].mark = 0;
      else
        landscape[idx].mark = 1;
      
      idx++;
      
      // ---
   
      copyVec( p0, landscape[idx].p0 );
      copyVec( p1, landscape[idx].p1 );
      copyVec( p3, landscape[idx].p2 );

      faceNormal( p0, p1, p3, n );
      
      copyVec( n, landscape[idx].n0 );
      copyVec( n, landscape[idx].n1 );
      copyVec( n, landscape[idx].n2 );
      copyVec( n, landscape[idx].n );
      
      if( equals( p0[3], 0.0f ) && equals( p1[3], 0.0f ) && equals( p3[3], 0.0f ) )
        landscape[idx].mark = 0;
      else
        landscape[idx].mark = 1;
      
      idx++;
		}
  
  // --- ---
  // smoothing

  int lineLength = maxCol << 1;
  for( idx = lineLength; idx < ( arraySize - lineLength ); )
  {
    // left border
    if( ( idx % lineLength == 0 ) )
    {
      // first triangle
      // ---
      // p1
      copyVec( landscape[idx].n, n );
      vecAdd( landscape[idx + 1].n, n, n );
      vecAdd( landscape[idx + lineLength].n, n, n );
      vecAdd( landscape[idx + lineLength + 3].n, n, n );
      vecAdd( landscape[idx + lineLength + 2].n, n, n );
      vecAdd( landscape[idx + 3].n, n, n );
      scalarMult( 1.0f / 6.0f, n, n );
      copyVec( n, landscape[idx].n1 );
      
      // ---
      // p2
      copyVec( landscape[idx].n, n );
      vecAdd( landscape[idx + 3].n, n, n );
      vecAdd( landscape[idx + 2].n, n, n );
      vecAdd( landscape[idx - lineLength + 3].n, n, n );
      vecAdd( landscape[idx - lineLength].n, n, n );
      vecAdd( landscape[idx - lineLength + 1].n, n, n );
      scalarMult( 1.0f / 6.0f, n, n );
      copyVec( n, landscape[idx].n2 );
      
      idx++;
      
      // second triangle
      // ---  
      // p2
      copyVec( landscape[idx-1].n1, landscape[idx].n2 );
      
      idx++;
      
      continue;
    }
    
    // right border
    if( ( idx + 2 ) % lineLength == 0 )
    {
      // first triangle
      // ---
      // p0
      copyVec( landscape[idx].n, n );
      vecAdd( landscape[idx - lineLength + 1].n, n, n );
      vecAdd( landscape[idx - lineLength - 1].n, n, n );
      vecAdd( landscape[idx - lineLength - 2].n, n, n );
      vecAdd( landscape[idx - 2].n, n, n );
      vecAdd( landscape[idx + 1].n, n, n );
      scalarMult( 1.0f / 6.0f, n, n );
      copyVec( n, landscape[idx].n0 );
      
      idx++;
      
      // second triangle
      // ---
      // p0
      copyVec( landscape[idx-1].n0, landscape[idx].n0 );
      
      // ---
      // p1
      copyVec( landscape[idx].n, n );
      vecAdd( landscape[idx - 3].n, n, n );
      vecAdd( landscape[idx - 2].n, n, n );
      vecAdd( landscape[idx + lineLength - 3].n, n, n );
      vecAdd( landscape[idx + lineLength].n, n, n );
      vecAdd( landscape[idx + lineLength - 1].n, n, n );
      scalarMult( 1.0f / 6.0f, n, n );
      copyVec( n, landscape[idx].n1 );
      
      idx++;
      
      continue;
    }
    
    // all center triangles
    
    // first triangle
    // ---
    // p0
    copyVec( landscape[idx].n, n );
    vecAdd( landscape[idx - lineLength + 1].n, n, n );
    vecAdd( landscape[idx - lineLength - 1].n, n, n );
    vecAdd( landscape[idx - lineLength - 2].n, n, n );
    vecAdd( landscape[idx - 2].n, n, n );
    vecAdd( landscape[idx + 1].n, n, n );
    scalarMult( 1.0f / 6.0f, n, n );
    copyVec( n, landscape[idx].n0 );
    
    // ---
    // p1
    copyVec( landscape[idx].n, n );
    vecAdd( landscape[idx + 1].n, n, n );
    vecAdd( landscape[idx + lineLength].n, n, n );
    vecAdd( landscape[idx + lineLength + 3].n, n, n );
    vecAdd( landscape[idx + lineLength + 2].n, n, n );
    vecAdd( landscape[idx + 3].n, n, n );
    scalarMult( 1.0f / 6.0f, n, n );
    copyVec( n, landscape[idx].n1 );
    
    // ---
    // p2
    copyVec( landscape[idx].n, n );
    vecAdd( landscape[idx + 3].n, n, n );
    vecAdd( landscape[idx + 2].n, n, n );
    vecAdd( landscape[idx - lineLength + 3].n, n, n );
    vecAdd( landscape[idx - lineLength].n, n, n );
    vecAdd( landscape[idx - lineLength + 1].n, n, n );
    scalarMult( 1.0f / 6.0f, n, n );
    copyVec( n, landscape[idx].n2 );

    idx++;
    
    // second triangle
    // ---
    // p0
    copyVec( landscape[idx-1].n0, landscape[idx].n0 );
    
    // ---
    // p1
    copyVec( landscape[idx].n, n );
    vecAdd( landscape[idx - 3].n, n, n );
    vecAdd( landscape[idx - 2].n, n, n );
    vecAdd( landscape[idx + lineLength - 3].n, n, n );
    vecAdd( landscape[idx + lineLength].n, n, n );
    vecAdd( landscape[idx + lineLength - 1].n, n, n );
    scalarMult( 1.0f / 6.0f, n, n );
    copyVec( n, landscape[idx].n1 );
    
    // ---
    // p2
    copyVec( landscape[idx-1].n1, landscape[idx].n2 );
    
    idx++;
  }
    
  // --- ---
  
  landscapeList = glGenLists( 1 );
  glNewList( landscapeList, GL_COMPILE );
  
  glBindTexture( GL_TEXTURE_2D, texture_ground );
  
  glBegin( GL_TRIANGLES );

  for( idx = 0; idx < arraySize; )
  {
    if( landscape[idx].mark )
    {
      glTexCoord2i( 0, 0 );
      glNormal3fv( landscape[idx].n0 );
      glVertex3fv( landscape[idx].p0 );
  
      glTexCoord2i( 1, 1 );
      glNormal3fv( landscape[idx].n1 );
      glVertex3fv( landscape[idx].p1 );
  
      glTexCoord2i( 1, 0 );
      glNormal3fv( landscape[idx].n2 );
      glVertex3fv( landscape[idx].p2 );
    }
    
    idx++;

    // ---    

    if( landscape[idx].mark )
    {
      glTexCoord2i( 0, 0 );
      glNormal3fv( landscape[idx].n0 );
      glVertex3fv( landscape[idx].p0 );
  
      glTexCoord2i( 0, 1 );
      glNormal3fv( landscape[idx].n1 );
      glVertex3fv( landscape[idx].p1 );
  
      glTexCoord2i( 1, 1 );
      glNormal3fv( landscape[idx].n2 );
      glVertex3fv( landscape[idx].p2 );
    }
    
    idx++;
  }

  glEnd();
	glEndList();
}

void landscape_draw()
{
  glPushAttrib( GL_ENABLE_BIT );
  
  glEnable( GL_TEXTURE_2D );
  
  glCallList( landscapeList );
  
  glPopAttrib();
}

float getHeightAtPos( float x, float z )
{
  // 'x' = ((float)col) * LANDSCAPE_FACTOR - xOff;
  // 'z' = ((float)row) * LANDSCAPE_FACTOR - zOff;
  
  float fX = ( (float)x + (float)X_OFFSET ) / (float)LANDSCAPE_FACTOR;
  float fZ = ( (float)z + (float)Z_OFFSET ) / (float)LANDSCAPE_FACTOR;
  
  int col = (int)floor( fX );
  int colNext = (int)ceil( fX );
  
  int row = (int)floor( fZ );
  int rowNext = (int)ceil( fZ );
  
  if( ( row > 0 ) && ( row < heightMap->height - 1 ) && ( col > 0 ) && ( col < heightMap->width ) )
  {
    const float valueRow = (heightMap->data[ (row * heightMap->width + col) * 4 ]) * LANDSCAPE_MAX_HEIGHT / 255.0f;
    const float valueRowNext = (heightMap->data[ (rowNext * heightMap->width + col) * 4 ]) * LANDSCAPE_MAX_HEIGHT / 255.0f;

    const float valueCol = (heightMap->data[ (row * heightMap->width + col) * 4 ]) * LANDSCAPE_MAX_HEIGHT / 255.0f;
    const float valueColNext = (heightMap->data[ (row * heightMap->width + colNext) * 4 ]) * LANDSCAPE_MAX_HEIGHT / 255.0f;

    float f0;
    float f1;
    
    float resultValue = 0.0f;
    
    f0 = fZ - row;
    f1 = 1.0f - f0;

    resultValue += 0.5f * ( ( f0 * valueRow ) + ( f1 * valueRowNext ) );
    
    // ---
    
    f0 = fX - col;
    f1 = 1.0f - f0;
    
    resultValue += 0.5f * ( ( f0 * valueCol ) + ( f1 * valueColNext ) );
    
    return resultValue;
  }
  
  return (heightMap->data[ (row * heightMap->width + col) * 4 ]) * LANDSCAPE_MAX_HEIGHT / 255.0;
}
