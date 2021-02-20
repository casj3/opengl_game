

#include "lib_convert.h"

glm::mat4 aiMatrix4x4ToGlm(aiMatrix4x4 from)
{
    glm::mat4 to;

    to[0][0] = from.a1; to[0][1] = from.b1;  to[0][2] = from.c1; to[0][3] = from.d1;
    to[1][0] = from.a2; to[1][1] = from.b2;  to[1][2] = from.c2; to[1][3] = from.d2;
    to[2][0] = from.a3; to[2][1] = from.b3;  to[2][2] = from.c3; to[2][3] = from.d3;
    to[3][0] = from.a4; to[3][1] = from.b4;  to[3][2] = from.c4; to[3][3] = from.d4;

    return to;
}

glm::mat3 aiMatrix3x3ToGlm(aiMatrix3x3 from)
{
    glm::mat4 to;

    to[0][0] = from.a1; to[0][1] = from.b1;  to[0][2] = from.c1; to[0][3] = 0;
    to[1][0] = from.a2; to[1][1] = from.b2;  to[1][2] = from.c2; to[1][3] = 0;
    to[2][0] = from.a3; to[2][1] = from.b3;  to[2][2] = from.c3; to[2][3] = 0;
    to[3][0] = 0;	to[3][1] = 0;        to[3][2] = 0;	 to[3][3] = 1;

    return to;
}

glm::vec3 aiVector3DtoGlm(aiVector3D from)
{
    glm::vec3 to = glm::vec3(from.x, from.y, from.z);

    return to;
}

glm::quat aiQuaternionToGlm(aiQuaternion from)
{
    glm::quat to = glm::quat(from.w, from.x, from.y, from.z);

    return to;
}
