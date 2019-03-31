#include "MathFunctions.h"

float VecDistance(vec3 pos1, vec3 pos2)
{
	vec3 dist;
	dist.x = pos1.x - pos2.x;
	dist.y = pos1.y - pos2.y;
	return length(dist);
}

void ComputeTangentSpace(NormalMapVertex* v0, NormalMapVertex* v1, NormalMapVertex* v2)
{
	// Lengyels method
	vec3 tangent, binormal, D, E;
	vec2 F, G;

	D = v1->position - v0->position;
	E = v2->position - v0->position;

	F = v1->tex_coords - v0->tex_coords;
	G = v2->tex_coords - v0->tex_coords;

	mat2 FG;
	FG[0][0] = F.x;
	FG[1][0] = F.y;
	FG[0][1] = G.x;
	FG[1][1] = G.y;

	FG = inverse(FG);

	tangent = { FG[0][0] * D.x + FG[1][0] * E.x, FG[0][0] * D.y + FG[1][0] * E.y, FG[0][0] * D.z + FG[1][0] * E.z };
	binormal = { FG[0][1] * D.x + FG[1][1] * E.x, FG[0][1] * D.y + FG[1][1] * E.y, FG[0][1] * D.z + FG[1][1] * E.z };

	v0->tangent += tangent;
	v1->tangent += tangent;
	v2->tangent += tangent;
	v0->binormal += binormal;
	v1->binormal += binormal;
	v2->binormal += binormal;
}

bool CircleCollision(vec3 pos1, vec3 pos2, float radius1, float radius2)
{
	if (VecDistance(pos1, pos2) < radius1 + radius2)
	{
		return true;
	}
	return false;
}

void ComputeNormalsPolygon(DefaultVertex* v0, DefaultVertex* v1, DefaultVertex* v2)
{
	vec3 vertex1 = v1->position - v0->position;
	vec3 vertex2 = v2->position - v0->position;

	vec3 normal = normalize(cross(vertex1, vertex2));

	v0->normal += normal;
	v1->normal += normal;
	v2->normal += normal;
}

// For flat quads only
void ComputeNormalsQuad(DefaultVertex* v0, DefaultVertex* v1, DefaultVertex* v2, DefaultVertex* v3)
{
	vec3 vertex1 = v1->position - v0->position;
	vec3 vertex2 = v2->position - v0->position;

	vec3 normal = normalize(cross(vertex1, vertex2));

	v0->normal += normal;
	v1->normal += normal;
	v2->normal += normal;
	v3->normal += normal;
}

bool DecomposeScale(mat4 matrix, vec3* scaling)
{
	scaling->x = length(matrix[0]);
	scaling->y = length(matrix[1]);
	scaling->z = length(matrix[2]);

	return true;
}

// TODO: This function generates errors still
bool DecomposeRotation(mat4 matrix, quat& rotation)
{
	// Normalize the matrix
	if (matrix[3][3] == 0)
		return false;

	for (length_t i = 0; i < 4; ++i)
	for (length_t j = 0; j < 4; ++j)
		matrix[i][j] /= matrix[3][3];

	float trace = matrix[0][0] + matrix[1][1] + matrix[2][2];

	if (trace > 0)
	{
		float s = 0.5f / sqrtf(trace + 1.0f);
		rotation.w = 0.25f / s;
		rotation.x = (matrix[1][2] - matrix[2][1]) * s;
		rotation.y = (matrix[2][0] - matrix[0][2]) * s;
		rotation.z = (matrix[0][1] - matrix[1][0]) * s;
	}
	else
	{
		if (matrix[0][0] > matrix[1][1] && matrix[0][0] > matrix[2][2])
		{
			float s = 2.0f * sqrtf(1.0f + matrix[0][0] - matrix[1][1] - matrix[2][2]);
			rotation.x = 0.25f * s;
			rotation.w = (matrix[1][2] - matrix[2][1]) / s;
			rotation.z = (matrix[2][0] + matrix[0][2]) / s;
			rotation.y = (matrix[1][0] + matrix[0][1]) / s;
		}
		else if (matrix[1][1] > matrix[2][2])
		{
			float s = 2.0f * sqrtf(1.0f + matrix[1][1] - matrix[0][0] - matrix[2][2]);
			rotation.y = 0.25f * s;
			rotation.w = (matrix[2][0] - matrix[0][2]) / s;
			rotation.z = (matrix[2][1] + matrix[1][2]) / s;
			rotation.x = (matrix[1][0] + matrix[0][1]) / s;
			/*float s = 2.0f * sqrtf(1.0f + a[1][1] - a[0][0] - a[2][2]);
			q.w = (a[0][2] - a[2][0]) / s;
			q.x = (a[0][1] + a[1][0]) / s;
			q.y = 0.25f * s;
			q.z = (a[1][2] + a[2][1]) / s;*/
		}
		else
		{
			float s = 2.0f * sqrtf(1.0f + matrix[2][2] - matrix[0][0] - matrix[1][1]);
			rotation.z = 0.25f * s;
			rotation.w = (matrix[0][1] - matrix[1][0]) / s;
			rotation.y = (matrix[2][1] + matrix[1][2]) / s;
			rotation.x = (matrix[2][0] + matrix[0][2]) / s;
			/*float s = 2.0f * sqrtf(1.0f + a[2][2] - a[0][0] - a[1][1]);
			q.w = (a[1][0] - a[0][1]) / s;
			q.x = (a[0][2] + a[2][0]) / s;
			q.y = (a[1][2] + a[2][1]) / s;
			q.z = 0.25f * s;*/
		}
	}

	return true;
}