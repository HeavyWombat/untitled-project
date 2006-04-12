#ifndef __GEOMETRY_H
#define __GEOMETRY_H

void geometry_multMatrices(float m1[16], float m2[16], float retMat[16]);
void geometry_multMatrixVector16x3(float m[16], float v[4], float retVec[4]);
void geometry_multMatrixVector16x4(float m[16], float v[4], float retVec[4]);
void geometry_getRotationMatrix(float angle, float xAxis, float yAxis, float zAxis, float* retMat);
void geometry_vector_add_3f(float v1[3], float v2[3], float retVal[3]);
void geometry_vector_sub_3f(float v1[3], float v2[3], float retVal[3]);
void geometry_vector_multScalar_3f(float s, float v[3], float retVal[3]);
float geometry_vector_dotProduct_3f(float v1[3], float v2[3]);
void geometry_vector_crossProduct_3f(float v1[3], float v2[3], float retVal[3]);
void geometry_vector_normalize(float v[3]);
int geometry_pointIsInsightTriangle(float point[3], float p1[3], float p2[3], float p3[3]);

#endif
