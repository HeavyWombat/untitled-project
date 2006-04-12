#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <gdv_os.h>

#include <camera.h>
#include <additionalMath.h>
#include <landscape.h>
#include <gdv_resources.h>
#include <trees.h>

// --- ---

static int treeList;

static const int arraySize0 = 26;
static const int arraySize1 = 22;
static const int arraySize2 = 10;
static const int arraySize3 = 10;

static const int arraySize = (arraySize0 << 1) + ( arraySize1 << 1 ) + (arraySize2 << 1) + arraySize3;
static tree trees[arraySize];

// --- ---

static void trees_drawTree( tree currentTree )
{
  float normal[3] = { 0.0f, 0.0f, 1.0f };
  float camera[3];
  float tree[3];
  float result[3];
  
  copyVec( cam.pos, camera );
  camera[1] = 0.0f;

  copyVec( currentTree.p, tree );
  tree[1] = 0.0f;
  
  vecSub( camera, tree, result );
  float angle = calc2PIAngle( normal, result ) * M_1_PI * 180.0;
  
  glPushMatrix();
  
  glPushAttrib( GL_ENABLE_BIT );
  
  glEnable( GL_BLEND );
  glEnable( GL_TEXTURE_2D );
  
  glBindTexture( GL_TEXTURE_2D, texture_tree[currentTree.type] );
  
  glTranslatef( currentTree.p[0], currentTree.p[1], currentTree.p[2] );
  glRotatef( angle, 0.0f, 1.0f, 0.0f );  
  
  glCallList( treeList );
  
  glPopAttrib();
   
  glEnd();
  
  glPopMatrix();
}

// --- ---

void trees_init( void )
{
  const float treeDim = 5.0f;
  const float eZ[3] = { 0.0f, 0.0f, 1.0f };
  
  treeList = glGenLists( 1 );
  glNewList( treeList, GL_COMPILE );
  
  glBegin( GL_QUADS );
  
  glNormal3fv( eZ );
  
  glTexCoord2d( 0, 1 );
  glVertex3f( -treeDim, 0.0f, 0.0f );
  
  glTexCoord2d( 1, 1 );
  glVertex3f(  treeDim, 0.0f, 0.0f );
  
  glTexCoord2d( 1, 0 );
  glVertex3f(  treeDim,  2.0f * treeDim, 0.0f );
  
  glTexCoord2d( 0, 0 );
  glVertex3f( -treeDim,  2.0f * treeDim, 0.0f );
  glEnd();
  
  glEndList();
  
  // ---
  
  srand( (unsigned int)gdv_os_getCurrentTime() );

  const float beforeLastRow = 248.0f - X_OFFSET;
  const float lastRow = 252.0f - X_OFFSET;
  const float relativeRows = 10.0f;
  
  int idx = 0;
  float offSetX;
  float offSetZ;
  float deltaX;
  float deltaZ;
  
  offSetX = 0.0f - X_OFFSET;
  offSetZ = -42.0f;
  for( int i = 0; i < arraySize0; i++ )
  {
    deltaX = 2.0f * ( 1.0f - ( rand() / ( RAND_MAX >> 1 ) ) );
    deltaZ = 2.0f * ( 1.0f - ( rand() / ( RAND_MAX >> 1 ) ) );
    
    trees[idx].p[0] = offSetX + (float)i * 10.0f + deltaX;
    trees[idx].p[2] = offSetZ + deltaZ; 
    trees[idx].p[1] = getHeightAtPos( trees[idx].p[0], trees[idx].p[2] );
    trees[idx].type = randomValue( 2 );
    
    idx++;
  }
  
  for( int i = 0; i < arraySize0; i++ )
  {
    deltaX = 2.0f * ( 1.0f - ( rand() / ( RAND_MAX >> 1 ) ) );
    deltaZ = 2.0f * ( 1.0f - ( rand() / ( RAND_MAX >> 1 ) ) );
    
    trees[idx].p[0] = offSetX + (float)i * 10.0f + deltaX;
    trees[idx].p[2] = offSetZ + deltaZ - relativeRows; 
    trees[idx].p[1] = getHeightAtPos( trees[idx].p[0], trees[idx].p[2] );
    trees[idx].type = randomValue( 2 );
    
    idx++;
  }
  
  // from top: right rows
  offSetX = 25.0f - X_OFFSET;
  offSetZ = -42.0f;
  for( int i = 0; i < arraySize1; i++ )
  {
    deltaX = 2.0f * ( 1.0f - ( rand() / ( RAND_MAX >> 1 ) ) );
    deltaZ = 2.0f * ( 1.0f - ( rand() / ( RAND_MAX >> 1 ) ) );
    
    trees[idx].p[0] = offSetX + (float)i * 10.0f + deltaX;
    trees[idx].p[2] = -offSetZ - deltaZ;
    trees[idx].p[1] = getHeightAtPos( trees[idx].p[0], trees[idx].p[2] );
    trees[idx].type = randomValue( 2 );
    
    idx++;
  }

  for( int i = 0; i < arraySize1; i++ )
  {
    deltaX = 2.0f * ( 1.0f - ( rand() / ( RAND_MAX >> 1 ) ) );
    deltaZ = 2.0f * ( 1.0f - ( rand() / ( RAND_MAX >> 1 ) ) );
    
    trees[idx].p[0] = offSetX + (float)i * 10.0f + deltaX;
    trees[idx].p[2] = -offSetZ - deltaZ + relativeRows;
    trees[idx].p[1] = getHeightAtPos( trees[idx].p[0], trees[idx].p[2] );
    trees[idx].type = randomValue( 2 );
    
    idx++;
  }
  
  // trees at the end
  offSetX = beforeLastRow;
  offSetZ = -35.0f;
  for( int i = 0; i < arraySize2; i++ )
  {
    deltaX = 1.0f * ( 1.0f - ( rand() / ( RAND_MAX >> 1 ) ) );
    deltaZ = 1.0f * ( 1.0f - ( rand() / ( RAND_MAX >> 1 ) ) );
    
    trees[idx].p[0] = offSetX + deltaX;
    trees[idx].p[2] = offSetZ + (float)i * 10.0f - deltaZ;
    trees[idx].p[1] = getHeightAtPos( trees[idx].p[0], trees[idx].p[2] );
    trees[idx].type = randomValue( 2 );
    
    idx++;
  }

  offSetX = lastRow;
  offSetZ = -38.0f;
  for( int i = 0; i < arraySize2; i++ )
  {
    deltaX = 1.0f * ( 1.0f - ( rand() / ( RAND_MAX >> 1 ) ) );
    deltaZ = 1.0f * ( 1.0f - ( rand() / ( RAND_MAX >> 1 ) ) );
    
    trees[idx].p[0] = offSetX + deltaX;
    trees[idx].p[2] = offSetZ + (float)i * 10.0f - deltaZ;
    trees[idx].p[1] = getHeightAtPos( trees[idx].p[0], trees[idx].p[2] );
    trees[idx].type = randomValue( 2 );
    
    idx++;
  }
  
  offSetX = - X_OFFSET;
  offSetZ = -38.0f;
  for( int i = 0; i < arraySize3; i++ )
  {
    deltaX = 0.01f * ( 1.0f - ( rand() / ( RAND_MAX >> 1 ) ) );
    deltaZ = 0.01f * ( 1.0f - ( rand() / ( RAND_MAX >> 1 ) ) );
    
    trees[idx].p[0] = offSetX + deltaX;
    trees[idx].p[2] = offSetZ + (float)i * 10.0f - deltaZ;
    trees[idx].p[1] = getHeightAtPos( trees[idx].p[0], trees[idx].p[2] );
    trees[idx].type = randomValue( 2 );
    
    idx++;
  }
}

void trees_draw( void )
{
  qsort( trees, arraySize, sizeof(tree), compareDist );
  
  for( int i=0; i<arraySize; i++ )
    trees_drawTree( trees[i] );
}

int compareDist( const void* t0, const void* t1 )
{
  tree tree0 = (tree)( *((tree*)t0) );
  tree tree1 = (tree)( *((tree*)t1) );
  
  float v0[3];
  float v1[3];
  
  vecSub( tree0.p, cam.pos, v0 );
  vecSub( tree1.p, cam.pos, v1 );
  
  double norm0 = v0[0] * v0[0] + v0[1] * v0[1] + v0[2] * v0[2];
  double norm1 = v1[0] * v1[0] + v1[1] * v1[1] + v1[2] * v1[2];
  
  if( norm0 < norm1 )
    return 1;
  
  if( norm0 > norm1 )
    return -1;
  
  return 0;
}
