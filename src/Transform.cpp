#include "Transform.h"





mat4 TransformMatrix(Transform transform)
{
	mat4 posMatrix = translate(transform.pos);
	mat4 rotXMatrix = rotate(transform.rot.x, vec3(1, 0, 0));
	mat4 rotYMatrix = rotate(transform.rot.y, vec3(0, 1, 0));
	mat4 rotZMatrix = rotate(transform.rot.z, vec3(0, 0, 1));
	mat4 scaleMatrix = scale(transform.scale);

	mat4 rotMatrix = rotZMatrix * rotYMatrix * rotXMatrix;

	return posMatrix * rotMatrix * scaleMatrix;
}