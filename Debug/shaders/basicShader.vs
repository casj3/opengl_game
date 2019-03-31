#version 330

attribute vec3 position;
attribute vec3 normal;
attribute vec2 texCoord;

out vec2 texCoord0;
out vec3 normal0;
out vec3 worldPos;

uniform mat4 world;
uniform mat4 view;
uniform mat4 perspective;

void main()
{	
	vec4 posL = vec4(position, 1.0);
	
	gl_Position = perspective * view * world * posL;
	texCoord0 = texCoord;
	vec4 normalL = vec4(normal, 0.0);
	normal0 = (world * normalL).xyz;
	worldPos = (world * posL).xyz;
}