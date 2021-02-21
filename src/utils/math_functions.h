#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "gpu_store.h"

#define degreesToRadians(x) x * (3.141592f/180.0f)

typedef unsigned long double ul_double;

float VecDistance(glm::vec3 pos1, glm::vec3 pos2);
ul_double GetFraction(ul_double numerator, ul_double denominator);
void ComputeTangentSpace(Normal_Map_Vertex* v0, Normal_Map_Vertex* v1, Normal_Map_Vertex* v2);
bool CircleCollision(glm::vec3 pos1, glm::vec3 pos2, float radius1, float radius2);
void ComputeNormalsPolygon(Default_Vertex* v0, Default_Vertex* v1, Default_Vertex* v2);
void ComputeNormalsQuad(Default_Vertex* v0, Default_Vertex* v1, Default_Vertex* v2, Default_Vertex* v3);
bool DecomposeScale(glm::mat4 matrix, glm::vec3* scaling);
// Note: Something is definitely wrong with this function
bool DecomposeRotation(glm::mat4 matrix, glm::quat& rotation);
