#version 330

struct CameraLightBuffer
{
	vec3 lightPos;
	vec3 cameraPos;
};

struct Material
{
	vec3 ka;
	vec3 kd;
	vec3 ks;
	float shininess;
};

uniform sampler2D diffuse;

in vec2 texCoord0;
in vec3 normal0;
in vec3 worldPos;

uniform sampler2D diffuseSamp1;
uniform sampler2D normalSamp1;

uniform Material material;

uniform CameraLightBuffer cameraLightBuffer;

out vec4 gl_FragColor;

void main()
{
	vec3 N = normalize(normal0);

	vec3 V = normalize(cameraLightBuffer.cameraPos - worldPos);
	vec3 L = normalize(cameraLightBuffer.lightPos - worldPos);
	vec3 R = reflect(-L, N);

	vec4 diff = vec4(material.kd * max(0.0, dot(L, N)), 0.0f);
	vec4 spec = vec4(material.ks * pow(max(0.0, dot(R, V)), material.shininess), 0.0f);

	gl_FragColor = vec4(material.ka, 0.0f) + diff + spec;
}
