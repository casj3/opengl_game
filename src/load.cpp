#include "load.h"

#include <cstring>

#include <GL/glew.h>
#include <glm/gtx/quaternion.hpp>
#include <assimp/postprocess.h>
#include "utils/lib_convert.h"
#include "transform.h"
#include "utils/print.h"
#include "utils/pair.h"

// TODO: Use a newer version of stb_image.h
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

constexpr uint32_t kFirstKeyFrame = 0;

struct AiBoneResources {
    aiMesh* mesh;
    aiAnimation* animation;
};

/// Holds the global values for a node given by its ID, for all key-frames.
struct NodeKeyFrameValues {
    const char* node_id;
    T_Append_Array<Pair_uint_vec3> positions;
    T_Append_Array<Pair_uint_vec3> rotations; // euler angles
    T_Append_Array<Pair_uint_vec3> scalings;
};

struct NodeMarkers {
    ARRAY_HANDLE(Pair_uint_uint) anim;
    ARRAY_HANDLE(Pair_uint_uint) bone;
};

void AddAnimationKeys(BoneAnimKeys* outAnimKeys,
                      T_Append_Array<Pair_uint_vec3> animComponent,
                      float keyFrameTime) {
    BoneAnimKeys animKeys = {
        GET_ARRAY_HANDLE(glm::vec3, animComponent.counter),
        GET_ARRAY_HANDLE(float, animComponent.counter),
    };

    for (uint32_t i = 0; i < animComponent.counter; i++) {
        (*animKeys.key_frames)[i] = (*animComponent.handle)[i].key * keyFrameTime;
        (*animKeys.values)[i] = (*animComponent.handle)[i].value;
    }

    *outAnimKeys = animKeys;
}

// Helper functions internal to the file
void RemoveNodeKeyFrameValues(ARRAY_HANDLE(NodeKeyFrameValues) nodes) {
    size_t arraySize = GET_ARRAY_SIZE(nodes);
    for(size_t i = 0; i < arraySize; i++) {
        NodeKeyFrameValues node = (*nodes)[i];
        RETURN_ARRAY(node.positions.handle);
        RETURN_ARRAY(node.rotations.handle);
        RETURN_ARRAY(node.scalings.handle);
    }
    RETURN_ARRAY(nodes);
}

void GetBoneAnimation(NodeKeyFrameValues node, glm::mat4* bone, Transform* transform, BoneAnimation* boneAnimation, float keyFrameTime);

/// Traverses skeleton and calculates the global positions of the bones per key frame.
///
/// @param boneNodes     Handle to array of bones and values identified by key-frames.
/// @param numKeyFrames  Amount of key-frames.
/// @param meshRoot      Mesh root node.
/// @param boneRes       Mesh and animation resources for the skeleton.
///
/// @return the node key frame values sorted in the correct order with regards to the aiMesh bone array.
ARRAY_HANDLE(NodeKeyFrameValues) ImportGlobalBonesThroughTime(ARRAY_HANDLE(NodeKeyFrameValues) boneNodes,
                                                             uint32_t numKeyFrames,
                                                             aiNode* meshRoot,
                                                             AiBoneResources boneRes);

void AddGlobalBonesForKeyFrame(aiNode* root,
                               aiMatrix4x4 parent,
                               AiBoneResources boneRes,
                               ARRAY_HANDLE(NodeKeyFrameValues) nodes,
                               NodeMarkers nodeMarkers,
                               uint32_t keyFrame,
                               uint32_t* animNodeCounter,
                               uint32_t* boneNodeCounter,
                               uint32_t* nodeCounter);

void AddGlobalBonesForFirstKeyFrame(aiNode* root,
                                    aiMatrix4x4 parent,
                                    AiBoneResources boneRes,
                                    ARRAY_HANDLE(NodeKeyFrameValues) nodes,
                                    T_Append_Array<Pair_uint_uint>* animNodeMarkers,
                                    T_Append_Array<Pair_uint_uint>* boneNodeMarkers,
                                    uint32_t numKeyFrames,
                                    uint32_t* nodeCounter);

void UpdateParentMatrix(aiMatrix4x4* parent, const aiNodeAnim* nodeAnim, uint32_t keyFrame);
Transform GetTransform(const aiMatrix4x4& matrix);

const aiNodeAnim* FindNodeAnim(aiAnimation* animation,
                               const char* nodeName,
                               T_Append_Array<Pair_uint_uint>* animNodeMarkers,
                               uint32_t nodeCounter);

bool IsBone(aiMesh* mesh,
            const char* nodeName,
            T_Append_Array<Pair_uint_uint>* boneNodeMarkers,
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
    for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        // Retrieve all indices of the face and store them in the indices std::vector
        for (uint32_t j = 0; j < face.mNumIndices; j++) {
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
Skeleton LoadSkeleton(const aiScene* scene, ARRAY_HANDLE(SkeletalVertex)* outVertices) {
    aiMesh* mesh = scene->mMeshes[0];
    ARRAY_HANDLE(SkeletalVertex) verticesHandle = GET_ARRAY_HANDLE(SkeletalVertex, mesh->mNumVertices);
    
    assert(mesh->mNumVertices == GET_ARRAY_SIZE(verticesHandle));

    SkeletalVertex* vertices = *verticesHandle;

    for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
        // Positions
        vertices[i].position.x = mesh->mVertices[i].x;
        vertices[i].position.y = mesh->mVertices[i].y;
        vertices[i].position.z = mesh->mVertices[i].z;

        // Normals
        vertices[i].normal.x = mesh->mNormals[i].x;
        vertices[i].normal.y = mesh->mNormals[i].y;
        vertices[i].normal.z = mesh->mNormals[i].z;

        // Texture Coordinates
        if (mesh->mTextureCoords[0]) { // Does the mesh contain texture coordinates?
            // A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vertices[i].tex_coords.x = mesh->mTextureCoords[0][i].x;
            vertices[i].tex_coords.y = mesh->mTextureCoords[0][i].y;
        }
        else {
            vertices[i].tex_coords = glm::vec2(0.0f, 0.0f);
        }
        // The undefined values in the arrays in the skeletal vertex need to be zero to signify their non-assignment.
        memset(vertices[i].IDs, 0, sizeof(vertices[i].IDs));
        memset(vertices[i].weights, 0, sizeof(vertices[i].weights));
    }

    // The largest amount of a transform component's keys set the roof for the total amount of key-frames for the animation
    uint32_t numPositionKeys = scene->mAnimations[0]->mChannels[0]->mNumPositionKeys;
    uint32_t numRotationKeys = scene->mAnimations[0]->mChannels[0]->mNumRotationKeys;
    uint32_t numScaleKeys = scene->mAnimations[0]->mChannels[0]->mNumScalingKeys;
    bool mostPositionKeys = numPositionKeys > numRotationKeys && numPositionKeys > numScaleKeys;
    bool mostRotationKeys = numRotationKeys > numPositionKeys && numRotationKeys > numScaleKeys;
    bool mostScaleKeys = numScaleKeys > numPositionKeys && numScaleKeys > numRotationKeys;
    uint32_t numKeyFrames = (numPositionKeys * mostPositionKeys +
                             numRotationKeys * mostRotationKeys +
                             mostScaleKeys * numScaleKeys) +
                             numPositionKeys * (!mostScaleKeys && !mostRotationKeys && !mostPositionKeys);

    // TODO: Support multiple animations per mesh. We should wait with this until we know what kinds of animations we have per type of mesh.
    // Doing one animation per mesh to begin with.
    NodeKeyFrameValues** nodes = GET_ARRAY_HANDLE(NodeKeyFrameValues, mesh->mNumBones);
    float animationSeconds = (float) (scene->mAnimations[0]->mDuration / scene->mAnimations[0]->mTicksPerSecond);
    float secondsPerKeyFrame = animationSeconds / (numKeyFrames - 1);

    Print("Number of key frames %u\n", numKeyFrames);
    Print("Seconds per key frame %.6f\n", secondsPerKeyFrame);
    Print("Animation second duration %.6f\n", animationSeconds);

    nodes = ImportGlobalBonesThroughTime(nodes, numKeyFrames, scene->mRootNode, { mesh, scene->mAnimations[0] });

    BoneAnimation** boneAnims = GET_ARRAY_HANDLE(BoneAnimation, mesh->mNumBones);
    Transform** boneTransforms = GET_ARRAY_HANDLE(Transform, mesh->mNumBones);
    glm::mat4** bones = GET_ARRAY_HANDLE(glm::mat4, mesh->mNumBones);

    for (uint32_t i = 0; i < mesh->mNumBones; i++) {
        GetBoneAnimation((*nodes)[i], (*bones) + i, (*boneTransforms) + i, (*boneAnims) + i, secondsPerKeyFrame);
        for (uint32_t j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
            uint32_t vertexId = mesh->mBones[i]->mWeights[j].mVertexId;
            float weight = mesh->mBones[i]->mWeights[j].mWeight;

            // This loops looks for a free slot to fill in the weight array of the concerned vertex
            for (uint32_t k = 0; k < sizeof(vertices[vertexId].IDs) / sizeof(vertices[vertexId].IDs[0]); k++) {
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

    *outVertices = verticesHandle;
    Skeleton skeleton = { boneAnims,
                          boneTransforms,
                          bones,
                          animationSeconds };

    return skeleton;
}

void GetBoneAnimation(NodeKeyFrameValues node, glm::mat4* bone, Transform* transform, BoneAnimation* boneAnimation, float keyFrameTime) {
    bool animatePos = node.positions.counter > 1;
    bool animateRot = node.rotations.counter > 1;
    bool animateScale = node.scalings.counter > 1;

    boneAnimation->animation_flags = GetAnimationFlags(animatePos, animateRot, animateScale);

    if (animatePos) {
        AddAnimationKeys(&boneAnimation->bone_position_keys, node.positions, keyFrameTime);
    }
    if (animateRot) {
        AddAnimationKeys(&boneAnimation->bone_rotation_keys, node.rotations, keyFrameTime);
    }
    if (animateScale) {
        AddAnimationKeys(&boneAnimation->bone_scale_keys, node.scalings, keyFrameTime);
    }

    *transform = { (*node.positions.handle)[0].value,
                   (*node.rotations.handle)[0].value,
                   (*node.scalings.handle)[0].value };

    // Add the starting bone to the bone array that will be used as the output destination.
    *bone = TransformMatrix(*transform);
}

ARRAY_HANDLE(NodeKeyFrameValues) ImportGlobalBonesThroughTime(ARRAY_HANDLE(NodeKeyFrameValues) boneNodes,
                                                              uint32_t numKeyFrames,
                                                              aiNode* meshRoot,
                                                              AiBoneResources boneRes) {
    // Keep track of the order of animation and skeletal nodes to avoid searching for them more than one time.
    NodeMarkers nodeMarkers;
    nodeMarkers.anim = GET_ARRAY_HANDLE(Pair_uint_uint, boneRes.animation->mNumChannels);
    nodeMarkers.bone = GET_ARRAY_HANDLE(Pair_uint_uint, boneRes.mesh->mNumBones);

    aiMatrix4x4 identityMatrix = aiMatrix4x4();
    uint32_t nodeCounter = 0;
    T_Append_Array<Pair_uint_uint> appAnimMarkers  = { nodeMarkers.anim, 0 };
    T_Append_Array<Pair_uint_uint> appBoneMarkers = { nodeMarkers.bone, 0 };
    // Record values for succeeding traversals of the node tree.
    AddGlobalBonesForFirstKeyFrame(meshRoot, identityMatrix, boneRes, boneNodes,
                                   &appAnimMarkers, &appBoneMarkers, numKeyFrames, &nodeCounter);

    uint32_t animNodeCounter;
    uint32_t boneNodeCounter;
    for(uint32_t i = kFirstKeyFrame + 1; i < numKeyFrames; i++) {
        nodeCounter = 0;
        animNodeCounter = 0;
        boneNodeCounter = 0;
        AddGlobalBonesForKeyFrame(meshRoot, identityMatrix, boneRes, boneNodes, nodeMarkers,
                                  i, &animNodeCounter, &boneNodeCounter, &nodeCounter);
    }

    RETURN_ARRAY(nodeMarkers.anim);
    RETURN_ARRAY(nodeMarkers.bone);

    return boneNodes;
}

/// Sets the elements identified by keyFrame if it is not the first one, i.e. 0.
void AddGlobalBonesForKeyFrame(aiNode* root,
                               aiMatrix4x4 parent,
                               AiBoneResources boneRes,
                               ARRAY_HANDLE(NodeKeyFrameValues) nodes,
                               NodeMarkers nodeMarkers,
                               uint32_t keyFrame,
                               uint32_t* animNodeCounter,
                               uint32_t* boneNodeCounter,
                               uint32_t* nodeCounter) {
    // If all bones have been retrieved, return.
    size_t numBones = GET_ARRAY_SIZE(nodeMarkers.bone);
    if (*boneNodeCounter >= numBones) {
        return;
    }

    // If the animation node counter hasn't been surpassed by
    size_t numAnimNodes = GET_ARRAY_SIZE(nodeMarkers.anim);
    if (numAnimNodes > *animNodeCounter && *nodeCounter == (*nodeMarkers.anim)[*animNodeCounter].key) {
        uint32_t animNodeId = (*nodeMarkers.anim)[*animNodeCounter].value;
        aiNodeAnim* nodeAnim = boneRes.animation->mChannels[animNodeId];
        UpdateParentMatrix(&parent, nodeAnim, keyFrame);

        // Increment counter to compare with the next entry in the array for the next node.
        (*animNodeCounter)++;
    } else {
        parent *= root->mTransformation;
    }

    Pair_uint_uint boneNodeMarker = (*nodeMarkers.bone)[*boneNodeCounter];
    if (*nodeCounter == boneNodeMarker.key) {
        NodeKeyFrameValues node = (*nodes)[boneNodeMarker.value];

        size_t prevPosId = TGetBack<>(node.positions);
        size_t prevRotId = TGetBack<>(node.rotations);
        size_t prevScaleId = TGetBack<>(node.scalings);

        Pair_uint_vec3 prevPos = (*node.positions.handle)[prevPosId];
        Pair_uint_vec3 prevRot = (*node.rotations.handle)[prevRotId];
        Pair_uint_vec3 prevScale = (*node.scalings.handle)[prevScaleId];

        // Multiply by the offset of the bone to get global space values.
        aiMatrix4x4 globalParent = parent * boneRes.mesh->mBones[boneNodeMarker.value]->mOffsetMatrix;
        Transform parentTransform = GetTransform(globalParent);

        // Only add the values if they have changed since the last key-frame.
        if (prevPos.value != parentTransform.pos) {
            // If there has been more than one key-frame in between the differing values, an extra key-frame
            // has to be added such that the interpolation occurs between the last key-frame differing from
            // the new one being added.
            if (keyFrame - prevPos.key > 1) {
                TAppendElement<>(&node.positions, { keyFrame - 1, prevPos.value });
            }
            TAppendElement<>(&node.positions, { keyFrame, parentTransform.pos });
        }
        if (prevRot.value != parentTransform.rot) {
            if (keyFrame - prevRot.key > 1) {
                TAppendElement<>(&node.rotations, { keyFrame - 1, prevRot.value });
            }
            TAppendElement<>(&node.rotations, { keyFrame, parentTransform.rot });
        }
        if (prevScale.value != parentTransform.scale) {
            if (keyFrame - prevScale.key > 1) {
                TAppendElement<>(&node.scalings, { keyFrame - 1, prevScale.value });
            }
            TAppendElement<>(&node.scalings, { keyFrame, parentTransform.scale });
        }

        // We mustn't forget to write back to the real node instance, since we're dealing with copies.
        (*nodes)[boneNodeMarker.value] = node;

        // Increment counter to compare with the next entry in the array for the next node.
        (*boneNodeCounter)++;
    }

    for (uint32_t i = 0; i < root->mNumChildren; i++)
    {
        (*nodeCounter)++;
        AddGlobalBonesForKeyFrame(root->mChildren[i],
                                  parent,
                                  boneRes,
                                  nodes,
                                  nodeMarkers,
                                  keyFrame,
                                  animNodeCounter,
                                  boneNodeCounter,
                                  nodeCounter);
    }
}

/// Sets the elements identified by the first keyFrame.
void AddGlobalBonesForFirstKeyFrame(aiNode* root,
                                    aiMatrix4x4 parent,
                                    AiBoneResources boneRes,
                                    ARRAY_HANDLE(NodeKeyFrameValues) nodes,
                                    T_Append_Array<Pair_uint_uint>* animNodeMarkers,
                                    T_Append_Array<Pair_uint_uint>* boneNodeMarkers,
                                    uint32_t numKeyFrames,
                                    uint32_t* nodeCounter) {
    const char* nodeName = root->mName.data;
    const aiNodeAnim* nodeAnim = FindNodeAnim(boneRes.animation, nodeName, animNodeMarkers, *nodeCounter);

    // A future error may reside in that if the keys for each aspect of the transform are multiple and differ
    if (nodeAnim) {
        UpdateParentMatrix(&parent, nodeAnim, kFirstKeyFrame);
    } else {
        parent *= root->mTransformation;
    }

    if (IsBone(boneRes.mesh, nodeName, boneNodeMarkers, *nodeCounter)) {
        NodeKeyFrameValues node = {
            nodeName,
            TGetAppendArray<Pair_uint_vec3>(numKeyFrames),
            TGetAppendArray<Pair_uint_vec3>(numKeyFrames),
            TGetAppendArray<Pair_uint_vec3>(numKeyFrames),
        };

        // The boneId is used to arrange the NodeKeyFrameValues in the same order as the assimp bone array, inherent to the aiMesh
        uint32_t boneId = (*boneNodeMarkers->handle)[TGetBack<>(*boneNodeMarkers)].value;

        // Multiply by the offset of the bone to get global space values.
        aiMatrix4x4 globalParent = parent * boneRes.mesh->mBones[boneId]->mOffsetMatrix;
        Transform parentTransform = GetTransform(globalParent);

        TAppendElement<>(&node.positions, { kFirstKeyFrame, parentTransform.pos });
        TAppendElement<>(&node.rotations, { kFirstKeyFrame, parentTransform.rot });
        TAppendElement<>(&node.scalings, { kFirstKeyFrame, parentTransform.scale });

        (*nodes)[boneId] = node;
    }

    for (uint32_t i = 0; i < root->mNumChildren; i++) {
        (*nodeCounter)++;
        AddGlobalBonesForFirstKeyFrame(root->mChildren[i],
                                       parent,
                                       boneRes,
                                       nodes,
                                       animNodeMarkers,
                                       boneNodeMarkers,
                                       numKeyFrames,
                                       nodeCounter);
    }
}

void UpdateParentMatrix(aiMatrix4x4* parent, const aiNodeAnim* nodeAnim, uint32_t keyFrame) {
    bool useKeyFrame = nodeAnim->mNumScalingKeys != 1 && keyFrame < nodeAnim->mNumScalingKeys;
    aiVector3D scaling = nodeAnim->mScalingKeys[useKeyFrame ? keyFrame : 0].mValue;

    useKeyFrame = nodeAnim->mNumRotationKeys != 1 && keyFrame < nodeAnim->mNumRotationKeys;
    aiQuaternion rotationQ = nodeAnim->mRotationKeys[useKeyFrame ? keyFrame : 0].mValue;

    useKeyFrame = nodeAnim->mNumPositionKeys != 1 && keyFrame < nodeAnim->mNumPositionKeys;
    aiVector3D translation = nodeAnim->mPositionKeys[useKeyFrame ? keyFrame : 0].mValue;

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
                               T_Append_Array<Pair_uint_uint>* animNodeMarkers,
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
            TAppendElement<>(animNodeMarkers, { nodeCounter, i });
            return nodeAnim;
        }
    }

    return NULL;
}

bool IsBone(aiMesh* mesh, const char* nodeName, T_Append_Array<Pair_uint_uint>* boneNodeMarkers, uint32_t nodeCounter) {
    for (uint32_t i = 0; i < mesh->mNumBones; i++) {
        if (strcmp(nodeName, mesh->mBones[i]->mName.data) == 0) {
            // The order in which the nodes are traversed is stored in this array.
            // Next time the nodes are traversed, this functions does not need to
            // be called again.
            TAppendElement<>(boneNodeMarkers, { nodeCounter, i });
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
