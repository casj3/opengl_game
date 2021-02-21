#include "gpu_program.h"

#include <Windows.h>
#include <fstream>
#include <GL/glew.h>
#include <assert.h>

#include "camera.h"

void CheckShaderError(uint32_t shader, uint32_t flag, bool isProgram);
uint32_t CreateShader(const char* source, uint32_t shaderType);
std::string LoadShader(std::string fileName);

void CreateDefaultProgram(Default_Program* shaders, Default_Uniforms* uniforms) {
    shaders->program = glCreateProgram();

    shaders->vertex_shader = CreateShader(
#include "shaders/basicShader.vs"
        , GL_VERTEX_SHADER);
    shaders->fragment_shader = CreateShader(
#include "shaders/basicShader.fs"
        , GL_FRAGMENT_SHADER);

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

void CreateSkinningProgram(Default_Program* shaders, Skinning_Uniforms* uniforms) {
    shaders->program = glCreateProgram();

    shaders->vertex_shader = CreateShader(
#include "shaders/skinningShader.vs"
        , GL_VERTEX_SHADER);
    shaders->fragment_shader = CreateShader(
#include "shaders/basicShader.fs"
        , GL_FRAGMENT_SHADER);

    glAttachShader(shaders->program, shaders->vertex_shader);
    glAttachShader(shaders->program, shaders->fragment_shader);

    glLinkProgram(shaders->program);
    CheckShaderError(shaders->program, GL_LINK_STATUS, true);

    glValidateProgram(shaders->program);
    CheckShaderError(shaders->program, GL_LINK_STATUS, true);

    uniforms->world_matrix = glGetUniformLocation(shaders->program, "world");
    uniforms->view_matrix = glGetUniformLocation(shaders->program, "view");
    uniforms->perspective_matrix = glGetUniformLocation(shaders->program, "perspective");

    for (uint32_t i = 0; i < sizeof(uniforms->bone_location) / sizeof(uniforms->bone_location[0]); i++) {
        char name[128];
        memset(name, 0, sizeof(name));
        _snprintf_s(name, sizeof(name), "bones[%d]", i);
        uniforms->bone_location[i] = glGetUniformLocation(shaders->program, name);
    }
}

uint32_t CreateShader(const char* source, uint32_t shaderType) {
    uint32_t shader = glCreateShader(shaderType);

    // Make sure there is a shader
    assert(shader != 0);

    const char* shader_source[1] = {source};

    const int shader_length[1] = { strlen(source) };

    glShaderSource(shader, 1, shader_source, shader_length);
    glCompileShader(shader);

    CheckShaderError(shader, GL_COMPILE_STATUS, false);

    return shader;
}

std::string LoadShader(std::string fileName) {
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


void CheckShaderError(uint32_t shader, uint32_t flag, bool isProgram) {
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

void Bind(uint32_t program) {
    glUseProgram(program);
}

void DestroyDefaultProgram(Default_Program program) {
    glDetachShader(program.program, program.vertex_shader);
    glDetachShader(program.program, program.fragment_shader);

    glDeleteShader(program.fragment_shader);
    glDeleteShader(program.vertex_shader);

    glDeleteProgram(program.program);
}

void UpdateUniforms(glm::mat4 world, glm::mat4 view, glm::mat4 perspective, Default_Uniforms uniforms) {
    glUniformMatrix4fv(uniforms.world_matrix, 1, GL_FALSE, &world[0][0]);
    glUniformMatrix4fv(uniforms.view_matrix, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(uniforms.perspective_matrix, 1, GL_FALSE, &perspective[0][0]);
}

// This method may have to be revised depending on how the value that the animation method returns
void UpdateSkinningUniforms(glm::mat4 world, Camera camera, Skeleton skeleton, Skinning_Uniforms uniforms) {
    glUniformMatrix4fv(uniforms.world_matrix, 1, GL_FALSE, &world[0][0]);
    glUniformMatrix4fv(uniforms.view_matrix, 1, GL_FALSE, &ViewMatrix(camera.view)[0][0]);
    glUniformMatrix4fv(uniforms.perspective_matrix, 1, GL_FALSE, &ProjectionMatrix(camera.projection)[0][0]);

    // It is important that the size of the bones doesn't supersede MAX_BONES
    size_t num_bones = GET_ARRAY_SIZE(skeleton.bones);
    assert(num_bones <= MAX_BONES);
    for (int i = 0; i < num_bones; i++)
    {
        glUniformMatrix4fv(uniforms.bone_location[i], 1, GL_FALSE, &(*skeleton.bones)[i][0][0]);
    }
}

void UpdateSkinningUniformsOrtho(glm::mat4 world, Ortho_Camera camera, Skeleton skeleton, Skinning_Uniforms uniforms) {
    glUniformMatrix4fv(uniforms.world_matrix, 1, GL_FALSE, &world[0][0]);
    glUniformMatrix4fv(uniforms.view_matrix, 1, GL_FALSE, &ViewMatrix(camera.view)[0][0]);
    glUniformMatrix4fv(uniforms.perspective_matrix, 1, GL_FALSE, &OrthoProjectionMatrix(camera.projection)[0][0]);

    // It is important that the size of the bones doesn't supersede MAX_BONES
    size_t num_bones = GET_ARRAY_SIZE(skeleton.bones);
    assert(num_bones <= MAX_BONES);
    for (size_t i = 0; i < num_bones; i++) {
        glUniformMatrix4fv(uniforms.bone_location[i], 1, GL_FALSE, &(*skeleton.bones)[i][0][0]);
    }
}

void UpdateWorldMatrix(glm::mat4 world, uint32_t world_matrix_uniform) {
    glUniformMatrix4fv(world_matrix_uniform, 1, GL_FALSE, &world[0][0]);
}

// TODO: Why copy to float[3]?
void UpdateCameraLightBuffer(glm::vec3 lightPos, glm::vec3 cameraPos, uint32_t program) {
    float light_floats[3] = { lightPos.x, lightPos.y, lightPos.z };
    float camera_floats[3] = { cameraPos.x, cameraPos.y, cameraPos.z };

    glUniform3fv(glGetUniformLocation(program, "cameraLightBuffer.lightPos"), 1, light_floats);
    glUniform3fv(glGetUniformLocation(program, "cameraLightBuffer.cameraPos"), 1, camera_floats);
}
