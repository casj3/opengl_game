#include "Shader.h"

#include <Windows.h>
#include <fstream>

#include <GL/glew.h>
#include <assert.h>

#include "Camera.h"

void CheckShaderError(unsigned int shader, unsigned int flag, bool isProgram);

void CreateDefaultProgram(ShaderProgramFiles shaderNames, DefaultProgram* shaders, DefaultUniforms* uniforms)
{
	shaders->program = glCreateProgram();
	shaders->vertex_shader = CreateShader(LoadShader(shaderNames.vertex_shader_file), GL_VERTEX_SHADER);
	shaders->fragment_shader = CreateShader(LoadShader(shaderNames.fragment_shader_file), GL_FRAGMENT_SHADER);

	glAttachShader(shaders->program, shaders->vertex_shader);
	glAttachShader(shaders->program, shaders->fragment_shader);

	glLinkProgram(shaders->program);
	CheckShaderError(shaders->program, GL_LINK_STATUS, true);

	glValidateProgram(shaders->program);
	CheckShaderError(shaders->program, GL_LINK_STATUS, true);

	uniforms->world_matrix = glGetUniformLocation(shaders->program, "world");
	uniforms->view_matrix = glGetUniformLocation(shaders->program, "view");
	uniforms->perspective_matrix = glGetUniformLocation(shaders->program, "perspective");
}

void CreateSkinningProgram(ShaderProgramFiles shaderNames, DefaultProgram* shaders, SkinningUniforms* uniforms)
{
	shaders->program = glCreateProgram();
	shaders->vertex_shader = CreateShader(LoadShader(shaderNames.vertex_shader_file), GL_VERTEX_SHADER);
	shaders->fragment_shader = CreateShader(LoadShader(shaderNames.fragment_shader_file), GL_FRAGMENT_SHADER);

	glAttachShader(shaders->program, shaders->vertex_shader);
	glAttachShader(shaders->program, shaders->fragment_shader);

	glLinkProgram(shaders->program);
	CheckShaderError(shaders->program, GL_LINK_STATUS, true);

	glValidateProgram(shaders->program);
	CheckShaderError(shaders->program, GL_LINK_STATUS, true);

	uniforms->world_matrix = glGetUniformLocation(shaders->program, "world");
	uniforms->view_matrix = glGetUniformLocation(shaders->program, "view");
	uniforms->perspective_matrix = glGetUniformLocation(shaders->program, "perspective");

	for (unsigned int i = 0; i < sizeof(uniforms->bone_location) / sizeof(uniforms->bone_location[0]); i++)
	{
		char Name[128];
		memset(Name, 0, sizeof(Name));
		_snprintf_s(Name, sizeof(Name), "bones[%d]", i);
		uniforms->bone_location[i] = glGetUniformLocation(shaders->program, Name);
	}
}

unsigned int CreateShader(std::string text, unsigned int shaderType)
{
	unsigned int shader = glCreateShader(shaderType);

	// Make sure there is a fucking shader
	assert(shader != 0);

	const char* shaderSourceStrings[1];
	int shaderSourceStringLengths[1];

	shaderSourceStrings[0] = text.c_str();
	shaderSourceStringLengths[0] = text.length();

	glShaderSource(shader, 1, shaderSourceStrings, shaderSourceStringLengths);
	glCompileShader(shader);

	CheckShaderError(shader, GL_COMPILE_STATUS, false);

	return shader;
}

std::string LoadShader(std::string fileName)
{
  std::ifstream file;
	file.open((fileName).c_str());

	std::string output;
	std::string line;

	assert(file.is_open());

	while (file.good())
	{
		getline(file, line);
		output.append(line + "\n");
	}

	return output;
}


void CheckShaderError(unsigned int shader, unsigned int flag, bool isProgram)
{
	int success = 0;
	char error[1024] = { 0 };

	if (isProgram)
	{
		glGetProgramiv(shader, flag, &success);
	}
	else
	{
		glGetShaderiv(shader, flag, &success);
	}

	if (success == GL_FALSE)
	{
		if (isProgram)
		{
			glGetProgramInfoLog(shader, sizeof(error), NULL, error);
		}
		else
		{
			glGetShaderInfoLog(shader, sizeof(error), NULL, error);
		}
		MessageBoxA(0, error, "OpenGL error message", MB_OK);
	}
}

void Bind(unsigned int program)
{
	glUseProgram(program);
}

void DestroyDefaultProgram(DefaultProgram program)
{
	glDetachShader(program.program, program.vertex_shader);
	glDetachShader(program.program, program.fragment_shader);

	glDeleteShader(program.fragment_shader);
	glDeleteShader(program.vertex_shader);

	glDeleteProgram(program.program);
}

void UpdateUniforms(glm::mat4 world, glm::mat4 view, glm::mat4 perspective, DefaultUniforms uniforms)
{
	glUniformMatrix4fv(uniforms.world_matrix, 1, GL_FALSE, &world[0][0]);
	glUniformMatrix4fv(uniforms.view_matrix, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(uniforms.perspective_matrix, 1, GL_FALSE, &perspective[0][0]);
}


// This method may have to be revised depending on how the value that the animation method returns
void UpdateSkinningUniforms(glm::mat4 world, Camera camera, SkeletonSuper skeleton, SkinningUniforms uniforms)
{
	glUniformMatrix4fv(uniforms.world_matrix, 1, GL_FALSE, &world[0][0]);
	glUniformMatrix4fv(uniforms.view_matrix, 1, GL_FALSE, &ViewMatrix(camera.view)[0][0]);
	glUniformMatrix4fv(uniforms.perspective_matrix, 1, GL_FALSE, &ProjectionMatrix(camera.projection)[0][0]);

	// It is important that the size of the bones doesn't supersede MAX_BONES
	for (int i = 0; i < skeleton.bone_amount; i++)
	{
		glUniformMatrix4fv(uniforms.bone_location[i], 1, GL_FALSE, &skeleton.animation_bone_transforms[i][0][0]);
	}
}

void UpdateSkinningUniformsOrtho(glm::mat4 world, OrthoCamera camera, SkeletonSuper skeleton, SkinningUniforms uniforms)
{
	glUniformMatrix4fv(uniforms.world_matrix, 1, GL_FALSE, &world[0][0]);
	glUniformMatrix4fv(uniforms.view_matrix, 1, GL_FALSE, &ViewMatrix(camera.view)[0][0]);
	glUniformMatrix4fv(uniforms.perspective_matrix, 1, GL_FALSE, &OrthoProjectionMatrix(camera.projection)[0][0]);

	// It is important that the size of the bones doesn't supersede MAX_BONES
	for (int i = 0; i < skeleton.bone_amount; i++)
	{
		glUniformMatrix4fv(uniforms.bone_location[i], 1, GL_FALSE, &skeleton.animation_bone_transforms[i][0][0]);
	}
}

void UpdateWorldMatrix(glm::mat4 world, unsigned int world_matrix_uniform)
{
	glUniformMatrix4fv(world_matrix_uniform, 1, GL_FALSE, &world[0][0]);
}

void UpdateCameraLightBuffer(glm::vec3 lightPos, glm::vec3 cameraPos, unsigned int program)
{
	float lightFloats[3] = { lightPos.x, lightPos.y, lightPos.z };
	float cameraFloats[3] = { cameraPos.x, cameraPos.y, cameraPos.z };

	glUniform3fv(glGetUniformLocation(program, "cameraLightBuffer.lightPos"), 1, lightFloats);
	glUniform3fv(glGetUniformLocation(program, "cameraLightBuffer.cameraPos"), 1, cameraFloats);
}
