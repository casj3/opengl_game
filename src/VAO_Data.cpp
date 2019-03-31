#include "VAO_Data.h"

void SetupDefaultVAO(uint* VAO, uint* VBO, uint* EBO, vector<DefaultVertex> vertices, vector<uint> indices)
{
	// Create buffers/arrays
	glGenVertexArrays(1, &*VAO);
	glGenBuffers(1, &*VBO);
	glGenBuffers(1, &*EBO);

	glBindVertexArray(*VAO);
	// Load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(DefaultVertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	// Set the vertex attribute pointers
	// Vertex Positions
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(DefaultVertex), (void *)0);
	// Vertex Normals
	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(DefaultVertex), (void *)offsetof(DefaultVertex, normal));
	// Vertex Texture Coords
	glEnableVertexAttribArray(TEX_COORD_LOCATION);
	glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(DefaultVertex), (void *)offsetof(DefaultVertex, tex_coords));

	glBindVertexArray(0);
}

void SetupSkeletalVAO(uint* VAO, uint* VBO, uint* EBO, vector<SkeletalVertex> vertices, vector<uint> indices)
{
	// Create buffers/arrays
	glGenVertexArrays(1, &*VAO);
	glGenBuffers(1, &*VBO);
	glGenBuffers(1, &*EBO);

	glBindVertexArray(*VAO);
	// Load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SkeletalVertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	// Set the vertex attribute pointers

	// Vertex Positions
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(SkeletalVertex), (void *)0);
	// Vertex Normals
	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(SkeletalVertex), (void *)offsetof(SkeletalVertex, normal));
	// Vertex Texture Coords
	glEnableVertexAttribArray(TEX_COORD_LOCATION);
	glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(SkeletalVertex), (void *)offsetof(SkeletalVertex, tex_coords));
	// Bone ID
	glEnableVertexAttribArray(BONE_ID_LOCATION);
	glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_INT, sizeof(SkeletalVertex), (void *)offsetof(SkeletalVertex, IDs));
	// Bone Weight
	glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
	glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(SkeletalVertex), (void *)offsetof(SkeletalVertex, weights));

	glBindVertexArray(0);
}

void DestroyVAO(uint* VAO)
{
	glDeleteVertexArrays(1, &*VAO);
}