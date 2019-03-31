#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "VAO_Data.h"

using namespace glm;

#define degreesToRadians(x) x * (3.141592f/180.0f)

float VecDistance(vec3 pos1, vec3 pos2);
void ComputeTangentSpace(NormalMapVertex* v0, NormalMapVertex* v1, NormalMapVertex* v2);
bool CircleCollision(vec3 pos1, vec3 pos2, float radius1, float radius2);
void ComputeNormalsPolygon(DefaultVertex* v0, DefaultVertex* v1, DefaultVertex* v2);
void ComputeNormalsQuad(DefaultVertex* v0, DefaultVertex* v1, DefaultVertex* v2, DefaultVertex* v3);
bool DecomposeScale(mat4 matrix, vec3* scaling);
// Note: Something is definitely wrong with this function
bool DecomposeRotation(mat4 matrix, quat& rotation);