#include "LoadVAO.h"

#include <GL/glew.h>
#include "utils/LibraryTranslations.h"
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const aiScene* ImportScene(Assimp::Importer* importer, std::string path)
{
	// Read file via ASSIMP
	const aiScene *scene = importer->ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	// Check for errors
	assert(scene && !scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE && scene->mRootNode);

	return scene;
}

std::vector<DefaultVertex> LoadVertices(aiMesh* mesh)
{
	std::vector<DefaultVertex> vertices;
	vertices.resize(mesh->mNumVertices);

	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		DefaultVertex vertex;
    glm::vec3 vector;

		// Positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		// Normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normal = vector;

		// Texture Coordinates
		if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.tex_coords = vec;
		}
		else
		{
			vertex.tex_coords = glm::vec2(0.0f, 0.0f);
		}

		vertices[i] = vertex;
	}
	return vertices;
}

std::vector<unsigned int> LoadIndices(aiMesh* mesh)
{
	std::vector<unsigned int> indices;

	// Now walk	through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// Retrieve all indices of the face and store them in the indices std::vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	return indices;
}

// This function has to be adapted to the new structure of arrays - Textures
Textures LoadTextures(aiMesh* mesh, const aiScene* scene, std::string path, Textures texturesLoaded)
{
	Textures textures;

	std::string directory = path.substr(0, path.find_last_of('/'));

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		Textures diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuseSamp", directory, texturesLoaded);
		textures.id.insert(textures.id.end(), diffuseMaps.id.begin(), diffuseMaps.id.end());
		textures.path.insert(textures.path.end(), diffuseMaps.path.begin(), diffuseMaps.path.end());
		textures.types.insert(textures.types.end(), diffuseMaps.types.begin(), diffuseMaps.types.end());

		Textures specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "specularSamp", directory, texturesLoaded);
		textures.id.insert(textures.id.end(), specularMaps.id.begin(), specularMaps.id.end());
		textures.path.insert(textures.path.end(), specularMaps.path.begin(), specularMaps.path.end());
		textures.types.insert(textures.types.end(), specularMaps.types.begin(), specularMaps.types.end());

		Textures normalMaps = LoadMaterialTextures(material, aiTextureType_NORMALS, "normalSamp", directory, texturesLoaded);
		textures.id.insert(textures.id.end(), normalMaps.id.begin(), normalMaps.id.end());
		textures.path.insert(normalMaps.path.end(), normalMaps.path.begin(), normalMaps.path.end());
		textures.types.insert(normalMaps.types.end(), normalMaps.types.begin(), normalMaps.types.end());
	}

	return textures;
}

Textures LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, std::string directory, Textures texturesLoaded)
{
	Textures textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;

		for (unsigned int i = 0; i < texturesLoaded.id.size(); i++)
		{
			if (texturesLoaded.path[i] == str)
			{
				textures.id.push_back(texturesLoaded.id[i]);
				textures.path.push_back(texturesLoaded.path[i]);
				textures.types.push_back(texturesLoaded.types[i]);

				// A texture with the same filepath has already been loaded, continue to next one. (optimization)
				skip = true;

				break;
			}
		}

		if (!skip)
		{
			// If texture hasn't been loaded already, load it
			textures.id.push_back(TextureFromFile(str.C_Str() + '/' + directory, directory));
			textures.types.push_back(typeName);
			textures.path.push_back(str);
			// Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			texturesLoaded.id.push_back(TextureFromFile(str.C_Str() + '/' + directory, directory));
			texturesLoaded.types.push_back(typeName);
			texturesLoaded.path.push_back(str);
		}
	}
	return textures;
}

unsigned int TextureFromFile(std::string fileName, std::string directory)
{
	int width, height, numComponents;

	unsigned char* imageData = stbi_load(fileName.c_str(), &width, &height, &numComponents, 4);

	unsigned int textureID;
	glGenTextures(1, &textureID);

	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(imageData);

	return textureID;
}

void DestroyTextures(Textures textures)
{
	for (unsigned int i = 0; i < textures.id.size(); i++)
	{
		glDeleteTextures(1, &textures.id[i]);
	}
}

std::vector<SkeletalVertex> LoadSkeletalVertices(aiMesh* mesh, std::vector<std::vector<glm::mat4>>* skeletonBoneFrames, const aiScene* scene)
{
	std::vector<SkeletalVertex> vertices;
	vertices.resize(mesh->mNumVertices);

	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		SkeletalVertex vertex;
    glm::vec3 vector;

		// Positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		// Normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normal = vector;

		// Texture Coordinates
		if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.tex_coords = vec;
		}
		else
		{
			vertex.tex_coords = glm::vec2(0.0f, 0.0f);
		}

		// The undefined values in the arrays in the skeletal vertex need to be zero in order for the assignment of values to work
		memset(vertex.IDs, 0, sizeof(vertex.IDs));
		memset(vertex.weights, 0, sizeof(vertex.weights));

		vertices[i] = vertex;
	}

	// Now the bones
	skeletonBoneFrames->resize(mesh->mNumBones);

	// This whole system relies upon a total match between the amount of key frames
  // and the time value within those key frames - an index to value correspondence
	unsigned int keyFrameSize = scene->mAnimations[0]->mChannels[0]->mNumPositionKeys - 1;

	for (unsigned int i = 0; i < mesh->mNumBones; i++)
	{
		std::string boneName(mesh->mBones[i]->mName.data);
		glm::mat4 boneOffset = aiMatrix4x4ToGlm(mesh->mBones[i]->mOffsetMatrix);

		(*skeletonBoneFrames)[i] = ImportGlobalBones(boneName, keyFrameSize, scene->mRootNode, boneOffset, scene->mAnimations[0]);

		for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
		{
			unsigned int vertex_ID = mesh->mBones[i]->mWeights[j].mVertexId;
			float weight = mesh->mBones[i]->mWeights[j].mWeight;

			// This loops looks for a free slot to fill in the weight array of the concerned vertex
			for (unsigned int k = 0; k < sizeof(vertices[vertex_ID].IDs) / sizeof(vertices[vertex_ID].IDs[0]); k++)
			{
				// 0.0 implies a free slot
				if (vertices[vertex_ID].weights[k] == 0.0)
				{
					vertices[vertex_ID].IDs[k] = i;
					vertices[vertex_ID].weights[k] = weight;
					break;
				}
			}
		}
	}

	return vertices;
}

std::vector<glm::mat4> ImportGlobalBones(std::string boneName, unsigned int keyFramesSize, aiNode* root, glm::mat4 offsetMatrix, aiAnimation* animation)
{
	std::vector<glm::mat4> globalBones;

	globalBones.resize(keyFramesSize);

	for (unsigned int i = 0; i < keyFramesSize; i++)
	{
    aiMatrix4x4* globalBoneTransform = GetTransform(i, root, boneName, aiMatrix4x4(), animation);
		// Copy the matrix to have the bones laid out sequentially in the vector
		globalBones[i] = aiMatrix4x4ToGlm(*globalBoneTransform) * offsetMatrix;

    free(globalBoneTransform);
	}

	return globalBones;
}

aiMatrix4x4* GetTransform(unsigned int keyFrame, aiNode* root, std::string boneName, aiMatrix4x4 parentTransform, aiAnimation* animation)
{
	aiMatrix4x4 nodeTransformation;

	std::string NodeName(root->mName.data);

	// Making the assumption that only one animation exists within the scene -
  // otherwise an animation index would be necessary as a parameter const
  // aiAnimation* pAnimation = scene->mAnimations[0];

	const aiNodeAnim* pNodeAnim = FindNodeAnim(animation, NodeName);

	// A future error may reside in that if the keys for each aspect of the transform are multiple and differ
	if (pNodeAnim)
	{
		// Interpolate scaling and generate scaling transformation
		aiVector3D scaling;
		if (pNodeAnim->mNumScalingKeys == 1)
		{
			scaling = pNodeAnim->mScalingKeys[0].mValue;
		}
		else // the amount is the total amount
		{
			scaling = pNodeAnim->mScalingKeys[keyFrame].mValue;
		}

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion rotationQ;
		if (pNodeAnim->mNumRotationKeys == 1)
		{
			rotationQ = pNodeAnim->mRotationKeys[0].mValue;
		}
		else // the amount is the total amount
		{
			rotationQ = pNodeAnim->mRotationKeys[keyFrame].mValue;
		}

		// Interpolate translation and generate translation transformation matrix (this is different because this assumes an exact correspondence)
		aiVector3D translation;
		if (pNodeAnim->mNumPositionKeys == 1)
		{
			translation = pNodeAnim->mPositionKeys[0].mValue;
		}
		else // the amount is the total amount
		{
			translation = pNodeAnim->mPositionKeys[keyFrame].mValue;
		}

		nodeTransformation = aiMatrix4x4(scaling, rotationQ, translation);
	}
	else
	{
		nodeTransformation = root->mTransformation;
	}

	if (boneName == NodeName)
	{
		return new aiMatrix4x4(parentTransform * nodeTransformation);
	}

	aiMatrix4x4* result;

	for (unsigned int i = 0; i < root->mNumChildren; i++)
	{
		result = GetTransform(keyFrame, root->mChildren[i], boneName, parentTransform * nodeTransformation, animation);
		if (result != nullptr)
		{
			return result;
		}
	}

	return nullptr;
}

const aiNodeAnim* FindNodeAnim(aiAnimation* pAnimation, std::string NodeName)
{
	for (unsigned int i = 0; i < pAnimation->mNumChannels; i++)
	{
		aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (std::string(pNodeAnim->mNodeName.data) == NodeName)
		{
			return pNodeAnim;
		}
	}

	return NULL;
}

unsigned int FindRotation(unsigned int AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
	{
		if (AnimationTime < pNodeAnim->mRotationKeys[i + 1].mTime)
		{
			return i;
		}
	}

  return 0;
}

unsigned int FindScaling(unsigned int AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
	{
		if (AnimationTime < pNodeAnim->mScalingKeys[i + 1].mTime)
		{
			return i;
		}
	}

  return 0;
}

unsigned int FindPosition(unsigned int AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
	{
		if (AnimationTime < pNodeAnim->mPositionKeys[i + 1].mTime)
		{
			return i;
		}
	}

  return 0;
}

Textures LoadMeshTextures(aiMesh* mesh, const aiScene* scene, std::string path)
{
	Textures textures;

	std::string directory = path.substr(0, path.find_last_of('/'));

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		Textures diffuseMaps = LoadMeshMaterialTextures(material, aiTextureType_DIFFUSE, "diffuseSamp", directory);
		textures.id.insert(textures.id.end(), diffuseMaps.id.begin(), diffuseMaps.id.end());
		textures.path.insert(textures.path.end(), diffuseMaps.path.begin(), diffuseMaps.path.end());
		textures.types.insert(textures.types.end(), diffuseMaps.types.begin(), diffuseMaps.types.end());

		Textures specularMaps = LoadMeshMaterialTextures(material, aiTextureType_SPECULAR, "specularSamp", directory);
		textures.id.insert(textures.id.end(), specularMaps.id.begin(), specularMaps.id.end());
		textures.path.insert(textures.path.end(), specularMaps.path.begin(), specularMaps.path.end());
		textures.types.insert(textures.types.end(), specularMaps.types.begin(), specularMaps.types.end());

		Textures normalMaps = LoadMeshMaterialTextures(material, aiTextureType_NORMALS, "normalSamp", directory);
		textures.id.insert(textures.id.end(), normalMaps.id.begin(), normalMaps.id.end());
		textures.path.insert(textures.path.end(), normalMaps.path.begin(), normalMaps.path.end());
		textures.types.insert(textures.types.end(), normalMaps.types.begin(), normalMaps.types.end());
	}

	return textures;
}

Textures LoadMeshMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, std::string directory)
{
	Textures textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		textures.id.push_back(TextureFromFile(str.C_Str() + '/' + directory, directory));
		textures.types.push_back(typeName);
		textures.path.push_back(str);
	}
	return textures;
}
