#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assimp/scene.h>

using namespace glm;

mat4 aiMatrix4x4ToGlm(aiMatrix4x4 from);
mat3 aiMatrix3x3ToGlm(aiMatrix3x3 from);
vec3 aiVector3DtoGlm(aiVector3D from);
quat aiQuaternionToGlm(aiQuaternion from);
aiMatrix4x4 TranslationMatrix(aiVector3D t);
// Unsure whether the scale matrix is actually set to an identity matrix in this case
aiMatrix4x4 ScalingMatrix(aiVector3D scale);