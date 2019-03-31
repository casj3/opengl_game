#include "Draw_VAO.h"


void DrawTexturedVAO(uint program, uint VAO, Textures textures, uint indicesSize)
{
	// Bind appropriate textures
	uint diffuseNr = 1;
	uint specularNr = 1;
	uint normalNr = 1;

	for (uint i = 0; i < textures.id.size(); i++)
	{
		// Retrieve texture number (the N in diffuse_textureN)
		stringstream ss;
		string number;
		string name = textures.types[i];

		if (name == "diffuseSamp")
		{
			ss << diffuseNr++; // Transfer uint to stream

			number = ss.str();
		}
		else if (name == "specularSamp")
		{
			ss << specularNr++; // Transfer uint to stream

			number = ss.str();
		}
		else if (name == "normalSamp")
		{
			ss << normalNr++; // Transfer uint to stream

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
	for (uint i = 0; i < textures.id.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void DrawTexturedPhongVAO(uint program, uint VAO, Textures textures, uint indicesSize, PhongMaterial material)
{
	// Bind appropriate textures
	uint diffuseNr = 1;
	uint specularNr = 1;
	uint normalNr = 1;

	for (uint i = 0; i < textures.id.size(); i++)
	{
		// Retrieve texture number (the N in diffuse_textureN)
		stringstream ss;
		string number;
		string name = textures.types[i];

		if (name == "diffuseSamp")
		{
			ss << diffuseNr++; // Transfer uint to stream

			number = ss.str();
		}
		else if (name == "specularSamp")
		{
			ss << specularNr++; // Transfer uint to stream

			number = ss.str();
		}
		else if (name == "normalSamp")
		{
			ss << normalNr++; // Transfer uint to stream

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
	for (uint i = 0; i < textures.id.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void DrawPhongVAO(uint program, uint VAO, uint indicesSize, PhongMaterial material)
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