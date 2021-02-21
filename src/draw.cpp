#include "draw.h"

#include <sstream>
#include <string>

#include <GL/glew.h>

void DrawTexturedVAO(uint32_t program, uint32_t VAO, Textures textures, uint32_t indicesSize)
{
    // Bind appropriate textures
    // 2019-07-27:
    // This needs to be revised, maybe simplified. I don't know what's going on with
    // all sstream crap here.
    uint32_t diffuse_nr = 1;
    uint32_t specular_nr = 1;
    uint32_t normal_nr = 1;

    for (uint32_t i = 0; i < textures.id.size(); i++)
    {
        // Retrieve texture number (the N in diffuse_textureN)
        std::stringstream ss;
        std::string number;
        std::string name = textures.types[i];

        if (name == "diffuseSamp")
        {
            ss << diffuse_nr++; // Transfer uint32_t to stream

            number = ss.str();
        }
        else if (name == "specularSamp")
        {
            ss << specular_nr++; // Transfer uint32_t to stream

            number = ss.str();
        }
        else if (name == "normalSamp")
        {
            ss << normal_nr++; // Transfer uint32_t to stream

            number = ss.str();
        }

        // Now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(program, (name + number).c_str()), i);

        // Active proper texture unit before binding
        glActiveTexture(GL_TEXTURE0 + i);

        // And finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textures.id[i]);
    }

    // Draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Always good practice to set everything back to defaults once configured.
    for (uint32_t i = 0; i < textures.id.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void DrawTexturedPhongVAO(uint32_t program, uint32_t VAO, Textures textures, uint32_t indicesSize, Phong_Material material)
{
    // Bind appropriate textures
    uint32_t diffuse_nr = 1;
    uint32_t specular_nr = 1;
    uint32_t normal_nr = 1;

    for (uint32_t i = 0; i < textures.id.size(); i++)
    {
        // Retrieve texture number (the N in diffuse_textureN)
        std::stringstream ss;
        std::string number;
        std::string name = textures.types[i];

        if (name == "diffuseSamp")
        {
            ss << diffuse_nr++; // Transfer uint32_t to stream

            number = ss.str();
        }
        else if (name == "specularSamp")
        {
            ss << specular_nr++; // Transfer uint32_t to stream

            number = ss.str();
        }
        else if (name == "normalSamp")
        {
            ss << normal_nr++; // Transfer uint32_t to stream

            number = ss.str();
        }

        // Now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(program, (name + number).c_str()), i);

        // Active proper texture unit before binding
        glActiveTexture(GL_TEXTURE0 + i);

        // And finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textures.id[i]);
    }

    // Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
    glUniform1f(glGetUniformLocation(program, "material.shininess"), material.shininess);

    glUniform3fv(glGetUniformLocation(program, "material.ka"), 1, material.ka);
    glUniform3fv(glGetUniformLocation(program, "material.kd"), 1, material.kd);
    glUniform3fv(glGetUniformLocation(program, "material.ks"), 1, material.ks);

    // Draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Always good practice to set everything back to defaults once configured.
    for (uint32_t i = 0; i < textures.id.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void DrawPhongVAO(uint32_t program, uint32_t VAO, uint32_t indicesSize, Phong_Material material)
{
    // Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
    glUniform1f(glGetUniformLocation(program, "material.shininess"), material.shininess);

    glUniform3fv(glGetUniformLocation(program, "material.ka"), 1, material.ka);
    glUniform3fv(glGetUniformLocation(program, "material.kd"), 1, material.kd);
    glUniform3fv(glGetUniformLocation(program, "material.ks"), 1, material.ks);

    // Draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
