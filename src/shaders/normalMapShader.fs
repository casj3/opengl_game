R"(
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

in vec2 texCoord0;
in vec3 normal0;
in vec3 worldPos;
in vec3 binormal0;
in vec3 tangent0;

uniform sampler2D diffuseSamp1;
uniform sampler2D normalSamp1;

uniform Material material;

uniform CameraLightBuffer cameraLightBuffer;

out vec4 gl_FragColor;

void main()
{
	vec3 n = normalize(normal0);
	vec3 b = normalize(binormal0);
	vec3 t = normalize(tangent0);

	vec3 tN = texture(normalSamp1, texCoord0).rgb * 2 - 1;

	mat3 TBN = mat3(t, b, n);

	vec3 N = normalize(TBN * tN);

	vec3 V = normalize(cameraLightBuffer.cameraPos - worldPos);
	vec3 L = normalize(cameraLightBuffer.lightPos - worldPos);
	vec3 R = reflect(-L, N);

	vec3 diff = material.kd * max(0.0, dot(L, N));
	vec3 spec = material.ks * pow(max(0.0, dot(R, V)), material.shininess);

	gl_FragColor = vec4(material.ka + diff + spec, 0);
}
)"
