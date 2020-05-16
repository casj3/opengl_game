#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assimp/matrix4x4.h>
#include <assimp/matrix3x3.h>
#include <assimp/quaternion.h>
#include <assimp/vector3.h>

glm::mat4 aiMatrix4x4ToGlm(aiMatrix4x4 from);
glm::mat3 aiMatrix3x3ToGlm(aiMatrix3x3 from);
glm::vec3 aiVector3DtoGlm(aiVector3D from);
glm::quat aiQuaternionToGlm(aiQuaternion from);
