#include<math.h>

#include<geometry.h>


void geometry_multMatrices(float m1[16], float m2[16], float retMat[16])
{
	retMat[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
	retMat[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
	retMat[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
	retMat[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];

	retMat[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
	retMat[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
	retMat[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
	retMat[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];

	retMat[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
	retMat[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
	retMat[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
	retMat[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];

	retMat[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
	retMat[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
	retMat[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
	retMat[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];
}

void geometry_multMatrixVector16x3(float m[16], float v[3], float retVec[3])
{
	float tempVec[3];
	tempVec[0] = v[0];
	tempVec[1] = v[1];
	tempVec[2] = v[2];

	retVec[0] = m[0] * tempVec[0] + m[4] * tempVec[1] + m[8] * tempVec[2];
	retVec[1] = m[1] * tempVec[0] + m[5] * tempVec[1] + m[9] * tempVec[2];
	retVec[2] = m[2] * tempVec[0] + m[6] * tempVec[1] + m[10] * tempVec[2];
}

void geometry_multMatrixVector16x4(float m[16], float v[4], float retVec[4])
{
	float tempVec[4];
	tempVec[0] = v[0];
	tempVec[1] = v[1];
	tempVec[2] = v[2];
	tempVec[3] = v[3];

	retVec[0] = m[0] * tempVec[0] + m[4] * tempVec[1] + m[8] * tempVec[2] + m[12] * tempVec[3];
	retVec[1] = m[1] * tempVec[0] + m[5] * tempVec[1] + m[9] * tempVec[2] + m[13] * tempVec[3];
	retVec[2] = m[2] * tempVec[0] + m[6] * tempVec[1] + m[10] * tempVec[2] + m[14] * tempVec[3];
	retVec[3] = m[3] * tempVec[0] + m[7] * tempVec[1] + m[11] * tempVec[2] + m[15] * tempVec[3];
}

void geometry_getRotationMatrix(float angle, float xAxis, float yAxis, float zAxis, float* retMat)
{
	float sinAngle = sin(angle);
	float cosAngle = cos(angle);

	retMat[0] = (1.0 - cosAngle) * xAxis * xAxis + cosAngle;
	retMat[1] = (1.0 - cosAngle) * xAxis * yAxis + sinAngle * zAxis;
	retMat[2] = (1.0 - cosAngle) * xAxis * zAxis - sinAngle * yAxis;
	retMat[3] = 0.0;

	retMat[4] = (1.0 - cosAngle) * xAxis * yAxis - sinAngle * zAxis;
	retMat[5] = (1.0 - cosAngle) * yAxis * yAxis + cosAngle;
	retMat[6] = (1.0 - cosAngle) * yAxis * zAxis + sinAngle * xAxis;
	retMat[7] = 0.0;

	retMat[8] = (1.0 - cosAngle) * xAxis * zAxis + sinAngle * yAxis;
	retMat[9] = (1.0 - cosAngle) * yAxis * zAxis - sinAngle * xAxis;
	retMat[10] = (1.0 - cosAngle) * zAxis * zAxis + cosAngle;
	retMat[11] = 0.0;

	retMat[12] = 0.0;
	retMat[13] = 0.0;
	retMat[14] = 0.0;
	retMat[15] = 1.0;
}

void geometry_vector_add_3f(float v1[3], float v2[3], float retVal[3])
{
	retVal[0] = v1[0] + v2[0];
	retVal[1] = v1[1] + v2[1];
	retVal[2] = v1[2] + v2[2];
}

void geometry_vector_sub_3f(float v1[3], float v2[3], float retVal[3])
{
	retVal[0] = v1[0] - v2[0];
	retVal[1] = v1[1] - v2[1];
	retVal[2] = v1[2] - v2[2];
}

void geometry_vector_multScalar_3f(float s, float v[3], float retVal[3])
{
	retVal[0] = s * v[0];
	retVal[1] = s * v[1];
	retVal[2] = s * v[2];
}

float geometry_vector_dotProduct_3f(float v1[3], float v2[3])
{
	float retVal;

	retVal = v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];

	return(retVal);
}

void geometry_vector_crossProduct_3f(float v1[3], float v2[3], float retVal[3])
{
	retVal[0] = v1[1] * v2[2] - v1[2] * v2[1];
	retVal[1] = v1[2] * v2[0] - v1[0] * v2[2];
	retVal[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void geometry_vector_normalize(float v[3])
{
	float length;
	length = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] /= length;
	v[1] /= length;
	v[2] /= length;
}

int geometry_pointIsInsightTriangle(float point[3], float p1[3], float p2[3], float p3[3])
{
	int isInside;

	float tmp1[3];
	float tmp2[3];
	float tmp3[3];

	float v1[3];
	float v2[3];
	float planeNormal[3];

	geometry_vector_sub_3f(p2, p1, v1);
	geometry_vector_sub_3f(p3, p1, v2);
	geometry_vector_crossProduct_3f(v1, v2, planeNormal);

	geometry_vector_sub_3f(point, p1, tmp1);
	geometry_vector_sub_3f(p2, p1, tmp2);
	geometry_vector_crossProduct_3f(tmp2, planeNormal, tmp3);

	isInside = geometry_vector_dotProduct_3f(tmp1, tmp3) < 0.0;

	geometry_vector_sub_3f(point, p2, tmp1);
	geometry_vector_sub_3f(p3, p2, tmp2);
	geometry_vector_crossProduct_3f(tmp2, planeNormal, tmp3);

	isInside = isInside && geometry_vector_dotProduct_3f(tmp1, tmp3) < 0.0; 

	geometry_vector_sub_3f(point, p3, tmp1);
	geometry_vector_sub_3f(p1, p3, tmp2);
	geometry_vector_crossProduct_3f(tmp2, planeNormal, tmp3);

	isInside = isInside && geometry_vector_dotProduct_3f(tmp1, tmp3) < 0.0; 
	
	return(isInside);
}
