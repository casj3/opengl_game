

#include "LibraryTranslations.h"



mat4 aiMatrix4x4ToGlm(aiMatrix4x4 from)
{
	mat4 to;

	to[0][0] = from.a1; to[0][1] = from.b1;  to[0][2] = from.c1; to[0][3] = from.d1;
	to[1][0] = from.a2; to[1][1] = from.b2;  to[1][2] = from.c2; to[1][3] = from.d2;
	to[2][0] = from.a3; to[2][1] = from.b3;  to[2][2] = from.c3; to[2][3] = from.d3;
	to[3][0] = from.a4; to[3][1] = from.b4;  to[3][2] = from.c4; to[3][3] = from.d4;

	return to;
}

mat3 aiMatrix3x3ToGlm(aiMatrix3x3 from)
{
	mat4 to;

	to[0][0] = from.a1; to[0][1] = from.b1;  to[0][2] = from.c1; to[0][3] = 0;
	to[1][0] = from.a2; to[1][1] = from.b2;  to[1][2] = from.c2; to[1][3] = 0;
	to[2][0] = from.a3; to[2][1] = from.b3;  to[2][2] = from.c3; to[2][3] = 0;
	to[3][0] = 0;		to[3][1] = 0;		 to[3][2] = 0;		 to[3][3] = 1;

	return to;
}

vec3 aiVector3DtoGlm(aiVector3D from)
{
	vec3 to = vec3(from.x, from.y, from.z);

	return to;
}

quat aiQuaternionToGlm(aiQuaternion from)
{
	quat to = quat(from.w, from.x, from.y, from.z);

	return to;
}

aiMatrix4x4 TranslationMatrix(aiVector3D t)
{
	aiMatrix4x4 Translate;
	Translate.a4 = t.x;
	Translate.b4 = t.y;
	Translate.c4 = t.z;

	return Translate;
}

// Unsure whether the scale matrix is actually set to an identity matrix in this case
aiMatrix4x4 ScalingMatrix(aiVector3D scale)
{
	aiMatrix4x4 Scale;

	Scale.a1 = scale.x;
	Scale.b2 = scale.y;
	Scale.c3 = scale.z;

	return Scale;
}

//Matrix(const aiMatrix4x4& AssimpMatrix) 
//{
//	this->m11 = AssimpMatrix.a1;
//	this->m12 = AssimpMatrix.a2;
//	this->m13 = AssimpMatrix.a3;
//	this->m14 = AssimpMatrix.a4;
//	this->m21 = AssimpMatrix.b1;
//	this->m22 = AssimpMatrix.b2;
//	this->m23 = AssimpMatrix.b3;
//	this->m24 = AssimpMatrix.b4;
//	this->m31 = AssimpMatrix.c1;
//	this->m32 = AssimpMatrix.c2;
//	this->m33 = AssimpMatrix.c3;
//	this->m34 = AssimpMatrix.c4;
//	this->m41 = AssimpMatrix.d1;
//	this->m42 = AssimpMatrix.d2;
//	this->m43 = AssimpMatrix.d3;
//	this->m44 = AssimpMatrix.d4;
//}