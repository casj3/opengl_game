#include "LoadVAO.h"

#include <cstring>

#include <GL/glew.h>
#include <glm/gtx/quaternion.hpp>
#include <assimp/postprocess.h>
#include "utils/ArrayManager.h"
#include "utils/LibraryTranslations.h"
#include "utils/Sort.h"
#include "Transform.h"

// TODO: Use the functions outlined in the OpenGL Programming Guide for texture loading and remove these two lines below.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

constexpr uint32_t kFirstKeyFrame = 0;

struct AiBoneResources {
    aiMesh* mesh;
    aiAnimation* animation;
};

struct NodeMarkers {
    ArrayHandle<Pair<uint32_t, uint32_t>> anim;
    ArrayHandle<Pair<uint32_t, uint32_t>> bone;
};

void AddAnimationKeys(BoneAnimKeys* outAnimKeys,
                      AppendArray<Pair<uint32_t, glm::vec3>> animComponent,
                      float keyFrameTime) {
    BoneAnimKeys animKeys = {
        AddArray<glm::vec3>(animComponent.counter),
        AddArray<float>(animComponent.counter),
    };

    for (uint32_t i = 0; i < animComponent.counter; i++) {
        animKeys.key_frames[i] = animComponent.handle[i].key * keyFrameTime;
        animKeys.values[i] = animComponent.handle[i].value;
    }

    *outAnimKeys = animKeys;
}

// Helper functions internal to the file
void RemoveNodeKeyFrameValues(ArrayHandle<NodeKeyFrameValues> nodes) {
    uint32_t arraySize = GetArraySize<>(nodes);
    for(uint32_t i = 0; i < arraySize; i++) {
        NodeKeyFrameValues node = nodes[i];
        RemoveArray<>(node.positions.handle);
        RemoveArray<>(node.rotations.handle);
        RemoveArray<>(node.scalings.handle);
    }
    RemoveArray<>(nodes);
}

void GetBoneAnimation(NodeKeyFrameValues node, glm::mat4* bone, BoneAnimation* boneAnimation, float keyFrameTime);

ArrayHandle<NodeKeyFrameValues> ImportGlobalBonesThroughTime(ArrayHandle<NodeKeyFrameValues> boneNodes,
                                                             uint32_t numKeyFrames,
                                                             aiNode* meshRoot,
                                                             AiBoneResources boneRes);

void AddGlobalBonesForKeyFrame(aiNode* root,
                               aiMatrix4x4* parent,
                               AiBoneResources boneRes,
                               ArrayHandle<NodeKeyFrameValues> nodes,
                               NodeMarkers nodeMarkers,
                               uint32_t keyFrame,
                               uint32_t animNodeCounter,
                               uint32_t boneNodeCounter,
                               uint32_t nodeCounter);

void AddGlobalBonesForFirstKeyFrame(aiNode* root,
                                    aiMatrix4x4* parent,
                                    AiBoneResources boneRes,
                                    AppendArray<NodeKeyFrameValues> nodes,
                                    AppendArray<Pair<uint32_t, uint32_t>> animNodeMarkers,
                                    AppendArray<Pair<uint32_t, uint32_t>> boneNodeMarkers,
                                    uint32_t numKeyFrames,
                                    uint32_t nodeCounter);

void UpdateParentMatrix(aiMatrix4x4* parent, const aiNodeAnim* nodeAnim, uint32_t keyFrame);
Transform GetTransform(const aiMatrix4x4& matrix);

const aiNodeAnim* FindNodeAnim(aiAnimation* animation,
                               const char* nodeName,
                               AppendArray<Pair<uint32_t, uint32_t>>* animNodeMarkers,
                               uint32_t nodeCounter);

bool IsBone(aiMesh* mesh,
            const char* nodeName,
            AppendArray<Pair<uint32_t, uint32_t>>* boneNodeMarkers,
            uint32_t nodeCounter);

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
    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
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

std::vector<uint32_t> LoadIndices(aiMesh* mesh)
{
    std::vector<uint32_t> indices;

    // Now walk	through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // Retrieve all indices of the face and store them in the indices std::vector
        for (uint32_t j = 0; j < face.mNumIndices; j++)
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

    for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        // Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;

        for (uint32_t i = 0; i < texturesLoaded.id.size(); i++)
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

uint32_t TextureFromFile(std::string fileName, std::string directory)
{
    int width, height, numComponents;

    unsigned char* imageData = stbi_load(fileName.c_str(), &width, &height, &numComponents, 4);

    uint32_t textureID;
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
    for (uint32_t i = 0; i < textures.id.size(); i++)
    {
        glDeleteTextures(1, &textures.id[i]);
    }
}

// TODO: Add documentation for clarifying that this function depends on scene to mesh correspondence.
Skeleton LoadSkeleton(const aiScene* scene, ArrayHandle<SkeletalVertex>* outVertices) {
    aiMesh* mesh = scene->mMeshes[0];
    ArrayHandle<SkeletalVertex> vertices = AddArray<SkeletalVertex>(mesh->mNumVertices);

    assert(mesh->mNumVertices == GetArraySize<SkeletalVertex>(vertices));

    for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
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

    // We rely upon a total match between the amount of key frames
    // and the time value within those key frames - an index to value correspondence. The models are baked.
    uint32_t numKeyFrames = scene->mAnimations[0]->mChannels[0]->mNumPositionKeys - 1;

    // TODO: Support multiple animations per mesh. We should wait with this until we know what kinds of animations we have per type of mesh.
    // Doing one animation per mesh to begin with.
    ArrayHandle<NodeKeyFrameValues> nodes = AddArray<NodeKeyFrameValues>(mesh->mNumBones);

    nodes = ImportGlobalBonesThroughTime(nodes, numKeyFrames, scene->mRootNode, { mesh, scene->mAnimations[0] });

    float animationSeconds = (float) (scene->mAnimations[0]->mDuration / scene->mAnimations[0]->mTicksPerSecond);
    float secondsPerKeyFrame = animationSeconds / numKeyFrames;

    ArrayHandle<BoneAnimation> boneAnims = AddArray<BoneAnimation>(mesh->mNumBones);
    ArrayHandle<glm::mat4> bones = AddArray<glm::mat4>(mesh->mNumBones);
    int32_t skeletonAnimationFlags = AnimationFlags::NONE;
    for (uint32_t i = 0; i < mesh->mNumBones; i++) {
        GetBoneAnimation(nodes[i], &bones[i], &boneAnims[i], secondsPerKeyFrame);
        skeletonAnimationFlags |= boneAnims[i].animation_flags;

        // TODO: Update the skeletonAnimationFlags here with the boneAnimationFlags.
        for (uint32_t j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
            uint32_t vertexId = mesh->mBones[i]->mWeights[j].mVertexId;
            float weight = mesh->mBones[i]->mWeights[j].mWeight;

            // This loops looks for a free slot to fill in the weight array of the concerned vertex
            for (uint32_t k = 0; k < sizeof(vertices[vertexId].IDs) / sizeof(vertices[vertexId].IDs[0]); k++)
            {
                // 0.0 implies a free slot
                if (vertices[vertexId].weights[k] == 0.0)
                {
                    vertices[vertexId].IDs[k] = i;
                    vertices[vertexId].weights[k] = weight;
                    break;
                }
            }
        }
    }
    // Remove what will not be needed outside of the function scope.
    RemoveNodeKeyFrameValues(nodes);

    *outVertices = vertices;
    Skeleton skeleton = { boneAnims, bones, skeletonAnimationFlags };
    return skeleton;
}

void GetBoneAnimation(NodeKeyFrameValues node, glm::mat4* bone, BoneAnimation* boneAnimation, float keyFrameTime) {
    bool animatePos = node.positions.counter > 1;
    bool animateRot = node.rotations.counter > 1;
    bool animateScale = node.scalings.counter > 1;

    boneAnimation->animation_flags = GetAnimationFlags(animatePos, animateRot, animateScale);

    // Stub out the initialization calls for the unwanted animation components.
    void (*addBoneAnimFunctions[true + 1])(BoneAnimKeys*, AppendArray<Pair<uint32_t, glm::vec3>>, float);

    // The animation flags should dictate what should be used, so the stubbed return value shouldn't matter.
    addBoneAnimFunctions[false] = [](BoneAnimKeys*, AppendArray<Pair<uint32_t, glm::vec3>>, float) {};
    addBoneAnimFunctions[true] = &AddAnimationKeys;

    addBoneAnimFunctions[animatePos](&boneAnimation->bone_position_keys, node.positions, keyFrameTime);
    addBoneAnimFunctions[animateRot](&boneAnimation->bone_rotation_keys, node.rotations, keyFrameTime);
    addBoneAnimFunctions[animateScale](&boneAnimation->bone_scale_keys, node.scalings, keyFrameTime);

    *bone = GetTransform(node.positions.handle[0].value, node.rotations.handle[0].value, node.scalings.handle[0].value);
}

/// Traverses skeleton and calculates the global positions of the bones per key frame.
///
/// @param boneNodes     Handle to array of bones and values identified by key-frames.
/// @param numKeyFrames  Amount of key-frames.
/// @param meshRoot      Mesh root node.
/// @param boneRes       Mesh and animation resources for the skeleton.
///
/// @return the node key frame values sorted in the correct order with regards to the aiMesh bone array.
ArrayHandle<NodeKeyFrameValues> ImportGlobalBonesThroughTime(ArrayHandle<NodeKeyFrameValues> boneNodes,
                                                             uint32_t numKeyFrames,
                                                             aiNode* meshRoot,
                                                             AiBoneResources boneRes) {
    // Keep track of the order of animation and skeletal nodes to avoid searching for them more than one time.
    // The number of animNodeMarkers is probably smaller than we allocate for, but since we don't know its size,
    // it will have to do.
    NodeMarkers nodeMarkers;
    nodeMarkers.anim = AddArray<Pair<uint32_t, uint32_t>>(boneRes.mesh->mNumBones);
    nodeMarkers.bone = AddArray<Pair<uint32_t, uint32_t>>(boneRes.mesh->mNumBones);

    // Record values for succeeding traversals of the node tree.
    AddGlobalBonesForFirstKeyFrame(meshRoot, &aiMatrix4x4(), boneRes, { boneNodes, }, { nodeMarkers.anim }, { nodeMarkers.bone }, numKeyFrames, 0);

    for(uint32_t i = kFirstKeyFrame + 1; i < numKeyFrames; i++) {
        AddGlobalBonesForKeyFrame(meshRoot, &aiMatrix4x4(), boneRes, boneNodes, nodeMarkers, i, 0, 0, 0);
    }

    // Use the AppendArrays to create the new key frame tables for scale, rotation and position per bone.
    Sort::RadixSortArrayLikePairs<NodeKeyFrameValues>(nodeMarkers.bone, boneNodes, boneRes.mesh->mNumBones);

    RemoveArray<>(nodeMarkers.anim);
    RemoveArray<>(nodeMarkers.bone);

    return boneNodes;
}

/// Sets the elements identified by keyFrame if it is not the first one, i.e. 0.
void AddGlobalBonesForKeyFrame(aiNode* root,
                               aiMatrix4x4* parent,
                               AiBoneResources boneRes,
                               ArrayHandle<NodeKeyFrameValues> nodes,
                               NodeMarkers nodeMarkers,
                               uint32_t keyFrame,
                               uint32_t animNodeCounter,
                               uint32_t boneNodeCounter,
                               uint32_t nodeCounter) {
    Pair<uint32_t, uint32_t> animNodeMarker = nodeMarkers.anim[animNodeCounter];
    if (nodeCounter == animNodeMarker.key) {
        aiNodeAnim* nodeAnim = boneRes.animation->mChannels[animNodeMarker.value];
        UpdateParentMatrix(parent, nodeAnim, keyFrame);

        // Increment counter to compare with the next entry in the array for the next node.
        // Only increment the counter if it's within array bounds.
        uint32_t numAnimNodes = GetArraySize<>(nodeMarkers.anim);
        animNodeCounter += animNodeCounter < numAnimNodes - 1;
    }

    Pair<uint32_t, uint32_t> boneNodeMarker = nodeMarkers.bone[boneNodeCounter];
    if (nodeCounter == boneNodeMarker.key) {
        NodeKeyFrameValues node = nodes[boneNodeCounter];

        uint32_t prevPosId = node.positions.counter - 1;
        uint32_t prevRotId = node.rotations.counter - 1;
        uint32_t prevScaleId = node.scalings.counter - 1;

        Pair<uint32_t, glm::vec3> prevPos = node.positions.handle[prevPosId];
        Pair<uint32_t, glm::vec3> prevRot = node.rotations.handle[prevRotId];
        Pair<uint32_t, glm::vec3> prevScale = node.scalings.handle[prevScaleId];

        // Multiply by the offset of the bone to get global space values.
        Transform parentTransform = GetTransform(*parent * boneRes.mesh->mBones[boneNodeMarker.value]->mOffsetMatrix);

        // Only add the values if they have changed since the last key-frame.
        if (prevPos.value != parentTransform.pos) {
            // If there has been more than one key-frame in between the differing values, an extra key-frame
            // has to be added such that the interpolation occurs between the last key-frame differing from
            // the new one being added.
            if (keyFrame - prevPos.key > 1) {
                node.positions = AppendElement<>(node.positions, prevPos);
            }
            node.positions = AppendElement<>(node.positions, { keyFrame, parentTransform.pos });
        }
        if (prevRot.value != parentTransform.rot) {
            if (keyFrame - prevRot.key > 1) {
                node.positions = AppendElement<>(node.rotations, prevRot);
            }
            node.rotations = AppendElement<>(node.rotations, { keyFrame, parentTransform.rot });
        }
        if (prevScale.value != parentTransform.scale) {
            if (keyFrame - prevScale.key > 1) {
                node.positions = AppendElement<>(node.scalings, prevScale);
            }
            node.scalings = AppendElement<>(node.scalings, { keyFrame, parentTransform.scale });
        }

        // We mustn't forget to write back to the real node instance, since we're dealing with copies.
        nodes[boneNodeCounter] = node;

        // Increment counter to compare with the next entry in the array for the next node.
        boneNodeCounter++;
    }

    // If all bones have been retrieved, return.
    uint32_t numBones = GetArraySize<Pair<uint32_t, uint32_t>>(nodeMarkers.bone);
    if (boneNodeCounter >= numBones) {
        return;
    }

    for (uint32_t i = 0; i < root->mNumChildren; i++)
    {
        AddGlobalBonesForKeyFrame(root->mChildren[i],
                                  parent,
                                  boneRes,
                                  nodes,
                                  nodeMarkers,
                                  keyFrame,
                                  animNodeCounter,
                                  boneNodeCounter,
                                  ++nodeCounter);
    }
}

/// Sets the elements identified by the first keyFrame.
void AddGlobalBonesForFirstKeyFrame(aiNode* root,
                                    aiMatrix4x4* parent,
                                    AiBoneResources boneRes,
                                    AppendArray<NodeKeyFrameValues> nodes,
                                    AppendArray<Pair<uint32_t, uint32_t>> animNodeMarkers,
                                    AppendArray<Pair<uint32_t, uint32_t>> boneNodeMarkers,
                                    uint32_t numKeyFrames,
                                    uint32_t nodeCounter) {
    const char* nodeName = root->mName.data;
    const aiNodeAnim* nodeAnim = FindNodeAnim(boneRes.animation, nodeName, &animNodeMarkers, nodeCounter);

    // A future error may reside in that if the keys for each aspect of the transform are multiple and differ
    if (nodeAnim) {
        UpdateParentMatrix(parent, nodeAnim, kFirstKeyFrame);
    }

    if (IsBone(boneRes.mesh, nodeName, &boneNodeMarkers, nodeCounter)) {
        NodeKeyFrameValues node = {
            nodeName,
            { AddArray<Pair<uint32_t, glm::vec3>>(numKeyFrames) },
            { AddArray<Pair<uint32_t, glm::vec3>>(numKeyFrames) },
            { AddArray<Pair<uint32_t, glm::vec3>>(numKeyFrames) },
        };

        uint32_t boneId = boneNodeMarkers.handle[boneNodeMarkers.counter - 1].value;

        // Multiply by the offset of the bone to get global space values.
        Transform parentTransform = GetTransform(*parent * boneRes.mesh->mBones[boneId]->mOffsetMatrix);

        node.positions = AppendElement<>(node.positions, { kFirstKeyFrame, parentTransform.pos });
        node.rotations = AppendElement<>(node.rotations, { kFirstKeyFrame, parentTransform.rot });
        node.scalings = AppendElement<>(node.scalings, { kFirstKeyFrame, parentTransform.scale });

        nodes = AppendElement<>(nodes, node);
    }

    for (uint32_t i = 0; i < root->mNumChildren; i++) {
        AddGlobalBonesForFirstKeyFrame(root->mChildren[i],
                                       parent,
                                       boneRes,
                                       nodes,
                                       animNodeMarkers,
                                       boneNodeMarkers,
                                       numKeyFrames,
                                       ++nodeCounter);
    }
}

void UpdateParentMatrix(aiMatrix4x4* parent, const aiNodeAnim* nodeAnim, uint32_t keyFrame) {
    aiVector3D scaling;
    if (nodeAnim->mNumScalingKeys == 1) {
        scaling = nodeAnim->mScalingKeys[0].mValue;
    }
    else {
        scaling = nodeAnim->mScalingKeys[keyFrame].mValue;
    }

    aiQuaternion rotationQ;
    if (nodeAnim->mNumRotationKeys == 1) {
        rotationQ = nodeAnim->mRotationKeys[0].mValue;
    }
    else {
        rotationQ = nodeAnim->mRotationKeys[keyFrame].mValue;
    }

    aiVector3D translation;
    if (nodeAnim->mNumPositionKeys == 1) {
        translation = nodeAnim->mPositionKeys[0].mValue;
    }
    else {
        translation = nodeAnim->mPositionKeys[keyFrame].mValue;
    }

    // Refresh parent for next node
    *parent *= aiMatrix4x4(scaling, rotationQ, translation);
}

Transform GetTransform(const aiMatrix4x4& matrix) {
    // Decompose to store the individual vectors since we don't want to store the global bones as matrices, since
    // that would take up unnecessary space in memory.
    aiVector3D scaling;
    aiQuaternion rotationQ;
    aiVector3D translation;
    matrix.Decompose(scaling, rotationQ, translation);

    Transform transform;
    transform.scale.x = scaling.x;
    transform.scale.y = scaling.y;
    transform.scale.z = scaling.z;

    glm::quat quat = glm::quat(rotationQ.w, rotationQ.x, rotationQ.y, rotationQ.z);
    glm::vec3 eulerAngles = glm::eulerAngles(quat);
    transform.rot.x = eulerAngles.x;
    transform.rot.y = eulerAngles.y;
    transform.rot.z = eulerAngles.z;

    transform.pos.x = translation.x;
    transform.pos.y = translation.y;
    transform.pos.z = translation.z;

    return transform;
}

const aiNodeAnim* FindNodeAnim(aiAnimation* animation,
                               const char* nodeName,
                               AppendArray<Pair<uint32_t, uint32_t>>* animNodeMarkers,
                               uint32_t nodeCounter) {
    for (uint32_t i = 0; i < animation->mNumChannels; i++)
    {
        aiNodeAnim* nodeAnim = animation->mChannels[i];

        if (strcmp(nodeAnim->mNodeName.data, nodeName) == 0)
        {
            // The order in which the nodes are traversed is stored in this array.
            // Next time the nodes are traversed, this functions does not need to
            // be called again, but instead a counter which is incremented for
            // every traversed node can be used for comparison against the next
            // value of the animNodeMarkers array.
            *animNodeMarkers = AppendElement<Pair<uint32_t, uint32_t>>(*animNodeMarkers, { nodeCounter, i });
            return nodeAnim;
        }
    }

    return NULL;
}

bool IsBone(aiMesh* mesh, const char* nodeName, AppendArray<Pair<uint32_t, uint32_t>>* boneNodeMarkers, uint32_t nodeCounter) {
    for (uint32_t i = 0; i < mesh->mNumBones; i++) {
        if (strcmp(nodeName, mesh->mBones[i]->mName.data) == 0) {
            // The order in which the nodes are traversed is stored in this array.
            // Next time the nodes are traversed, this functions does not need to
            // be called again.
            *boneNodeMarkers = AppendElement<Pair<uint32_t, uint32_t>>(*boneNodeMarkers, { nodeCounter, i });
            return true;
        }
    }
    return false;
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

    for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        textures.id.push_back(TextureFromFile(str.C_Str() + '/' + directory, directory));
        textures.types.push_back(typeName);
        textures.path.push_back(str);
    }
    return textures;
}
