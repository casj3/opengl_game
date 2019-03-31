#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <fstream>
#include <vector>
#include "Animation.h"
#include "Camera.h"

//typedef unsigned int uint;

#define MAX_BONES 25

using namespace std;
using namespace glm;

struct DefaultProgram
{
	uint vertex_shader;
	uint fragment_shader;

	uint program;
};

// As soon there exists more program structures, this union becomes relevant
union Program
{
	DefaultProgram defaultProgram;
	// to be continued ...
};

struct ShaderProgramFiles
{
	string vertex_shader_file;
	string fragment_shader_file;
};

struct DefaultUniforms
{
	uint world_matrix;
	uint view_matrix;
	uint perspective_matrix;
};

struct SkinningUniforms
{
	uint world_matrix;
	uint view_matrix;
	uint perspective_matrix;
	uint bone_location[MAX_BONES];
};

// A union only takes up space for its most recently assigned variable, which makes it very useful in the superstructure to come
union Uniforms
{
	SkinningUniforms skinningUniforms;
	DefaultUniforms defaultUniforms;
};

// The superstructure is used to group arguments meant for a specific module - in this case the shader/program one
struct ProgramSuper
{
	Uniforms uniforms_type;
	Program program_type;
};

void CreateDefaultProgram(ShaderProgramFiles shaderNames, DefaultProgram* shaders, DefaultUniforms* uniforms);
void CreateSkinningProgram(ShaderProgramFiles shaderNames, DefaultProgram* shaders, SkinningUniforms* uniforms);

uint CreateShader(string text, GLenum shaderType);
string LoadShader(string fileName);
void Bind(uint program);
void DestroyDefaultProgram(DefaultProgram program);

void UpdateUniforms(mat4 world, mat4 view, mat4 perspective, DefaultUniforms uniforms);
void UpdateSkinningUniforms(mat4 world, Camera camera, SkeletonSuper skeleton, SkinningUniforms uniforms);
void UpdateSkinningUniformsOrtho(mat4 world, OrthoCamera camera, SkeletonSuper skeleton, SkinningUniforms uniforms);
void UpdateWorldMatrix(mat4 world, uint world_matrix_uniform);
void UpdateCameraLightBuffer(glm::vec3 lightPos, glm::vec3 cameraPos, uint program);
