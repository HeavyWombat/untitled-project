#ifndef ADDITIONALMATH_H_
#define ADDITIONALMATH_H_

void copyVec( float* srcVec, float* destVec );
double scalar( float* p0, float* p1 );
double norm( float* v );
double calcAngle( float* p0, float* p1 );
double calc2PIAngle( float* p0, float* p1 );
void vecAdd( float* v0, float* v1, float* v );
void vecSub( float* v0, float* v1, float* v );
void scalarMult( float scalar, float* v0, float* v );
void crossProduct( float* v0, float* v1, float* v );
void normalize( float* v );
void faceNormal( float* p0, float* p1, float* p2, float* normal );
int equals( float f0, float f1 );
float calcMagnifierAngle( float mag );
float arcToDeg( float arc );
float degToArc( float deg );
int randomValue( int number );
float randomFloatNegPos( void );
float randomFloatPos( void );

#endif /*ADDITIONALMATH_H_*/
