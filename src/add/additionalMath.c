#include <stdlib.h>
#include <math.h>

#include <cameraMovement.h>
#include <additionalMath.h>

// --- ---

// --- ---

// --- ---

void copyVec( float* srcVec, float* destVec )
{
  destVec[0] = srcVec[0];
  destVec[1] = srcVec[1];
  destVec[2] = srcVec[2];    
}

double scalar( float* p0, float* p1 )
{
  return ( (double)p0[0] * (double)p1[0] ) + ( (double)p0[1] * (double)p1[1] ) + ( (double)p0[2] * (double)p1[2] );
}

double norm( float* v )
{
  return sqrt( ( (double)v[0] * (double)v[0] ) + ( (double)v[1] * (double)v[1] ) + ( (double)v[2] * (double)v[2] ) );
}

double calcAngle( float* p0, float* p1 )
{
  return acos( scalar( p0, p1 ) / ( norm( p0 ) * norm( p1 ) ) );
}

double calc2PIAngle( float* p0, float* p1 )
{ 
  double angle = calcAngle( p0, p1 ) ;
  return ( p1[0] > 0.0f )? angle: 2.0 * M_PI - angle;
}

void vecAdd( float* v0, float* v1, float* v )
{
  v[0] = v0[0] + v1[0];
  v[1] = v0[1] + v1[1];
  v[2] = v0[2] + v1[2];
}

void vecSub( float* v0, float* v1, float* v )
{
  v[0] = v0[0] - v1[0];
  v[1] = v0[1] - v1[1];
  v[2] = v0[2] - v1[2];
}

void scalarMult( float scalar, float* v0, float* v )
{
  v[0] = scalar * v0[0];
  v[1] = scalar * v0[1];
  v[2] = scalar * v0[2];
}

void crossProduct( float* v0, float* v1, float* v )
{
  v[0] = ( v0[1] * v1[2] ) - ( v0[2] * v1[1] );  
  v[1] = ( v0[2] * v1[0] ) - ( v0[0] * v1[2] );
  v[2] = ( v0[0] * v1[1] ) - ( v0[1] * v1[0] );
}

void normalize( float* v )
{
  const float reLength = 1.0f / norm( v );
  
  v[0] *= reLength;
  v[1] *= reLength;
  v[2] *= reLength;
}

void faceNormal( float* p0, float* p1, float* p2, float* normal )
{
  float v0[3];
  float v1[3];
  
  vecSub( p1, p0, v0 );
  vecSub( p2, p0, v1 );
  crossProduct( v0, v1, normal );
  normalize( normal );
}

int equals( float f0, float f1 )
{
  const float delta = 1.0e-5;
  
  return ((f0 + delta) > f1 ) && ((f0 - delta) < f1);
}

float calcMagnifierAngle( float mag )
{
  const float factor = tan( degToArc( defaultFOV ) );
  
  const float magFactor = 1.0f / mag;
  
  return arcToDeg( atan( magFactor * factor ) );
}

float arcToDeg( float arc )
{
  return arc * 180.0f * M_1_PI;
}

float degToArc( float deg )
{  
  return deg / 180.0f * M_PI;
}

int randomValue( int number )
{
  return (int)( floor( (double)number * (double)rand() / (double)RAND_MAX ) );
}

float randomFloatNegPos( void )
{
  return ( 2.0f * (float)rand() / (float)RAND_MAX ) - 1.0f;
}

float randomFloatPos( void )
{
  return (float)rand() / (float)RAND_MAX;
}
