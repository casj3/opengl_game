#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in ivec4 boneIDs;
layout(location = 4) in vec4 weights;

out vec2 texCoord0;
out vec3 normal0;
out vec3 worldPos;

const int MAX_BONES = 25;

uniform mat4 world;
uniform mat4 view;
uniform mat4 perspective;
uniform mat4 bones[MAX_BONES];

void main()
{
	mat4 boneTransform = bones[boneIDs[0]] * weights[0];
    boneTransform     += bones[boneIDs[1]] * weights[1];
    boneTransform     += bones[boneIDs[2]] * weights[2];
    boneTransform     += bones[boneIDs[3]] * weights[3];

	vec4 posL = boneTransform * vec4(position, 1.0);

	// vec4 posL = vec4(position, 1.0);

	gl_Position = perspective * view * world * posL;

	texCoord0 = texCoord;

	vec4 normalL = boneTransform * vec4(normal, 0.0);

	// vec4 normalL = vec4(normal, 0.0);

	normal0 = (world * normalL).xyz;
	worldPos = (world * posL).xyz;
}
