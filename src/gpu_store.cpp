#include "gpu_store.h"

#include <GL/glew.h>

void SetupDefaultVAO(uint32_t* VAO, uint32_t* VBO, uint32_t* EBO, std::vector<Default_Vertex> vertices, std::vector<uint32_t> indices)
{
    // Create buffers/arrays
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);
    glGenBuffers(1, EBO);

    glBindVertexArray(*VAO);
    // Load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Default_Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    // Vertex Positions
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Default_Vertex), (void *)0);
    // Vertex Normals
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Default_Vertex), (void *)offsetof(Default_Vertex, normal));
    // Vertex Texture Coords
    glEnableVertexAttribArray(TEX_COORD_LOCATION);
    glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(Default_Vertex), (void *)offsetof(Default_Vertex, tex_coords));

    glBindVertexArray(0);
}

void SetupSkeletalVAO(uint32_t* VAO, uint32_t* VBO, uint32_t* EBO, ARRAY_HANDLE(Skeletal_Vertex) vertices, std::vector<uint32_t> indices)
{
    // Create buffers/arrays
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);
    glGenBuffers(1, EBO);

    glBindVertexArray(*VAO);
    // Load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, GET_ARRAY_SIZE(vertices) * sizeof(Skeletal_Vertex), *vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    // Set the vertex attribute pointers

    // Vertex Positions
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Skeletal_Vertex), (void *)0);
    // Vertex Normals
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Skeletal_Vertex), (void *)offsetof(Skeletal_Vertex, normal));
    // Vertex Texture Coords
    glEnableVertexAttribArray(TEX_COORD_LOCATION);
    glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(Skeletal_Vertex), (void *)offsetof(Skeletal_Vertex, tex_coords));
    // Bone ID
    glEnableVertexAttribArray(BONE_ID_LOCATION);
    glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_INT, sizeof(Skeletal_Vertex), (void *)offsetof(Skeletal_Vertex, IDs));
    // Bone Weight
    glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
    glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(Skeletal_Vertex), (void *)offsetof(Skeletal_Vertex, weights));

    glBindVertexArray(0);
}

void DestroyVAO(uint32_t* VAO)
{
    glDeleteVertexArrays(1, VAO);
}
