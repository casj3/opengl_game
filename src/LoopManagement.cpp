#include "LoopManagement.h"

#include <vector>

#include <glm/glm.hpp>

#include <utils/ArrayManager.h>
#include "utils/MathFunctions.h"
#include "VAO_Data.h"
#include "enums.h"

// Global variables which are declared as extern in different header files
Input currentUpdate;
Input lastUpdate;
MouseWheel mouseWheel;
// TODO: Revise how this is being used, most of the time the process exits heap allocation errors pop up.
Assimp::Importer importer;

// TODO: Can't load scenes like this.
void InitializeScene1(ProgramSuper* program, struct Skeletons* skeletons, UserSuper* user) {
    std::string avatarPath = "./assets/models/test.fbx";
    const aiScene* scene = ImportScene(&importer, avatarPath);

    // All the vertices and indices to create a VAO
    ArrayHandle<SkeletalVertex> skeletalVertices;
    Skeleton skeleton = LoadSkeleton(scene, &skeletalVertices);
    std::vector<uint32_t> indices = LoadIndices(scene->mMeshes[0]);

    // TODO: Revise what to do with the VBO and EBO.
    uint32_t VAO, VBO, EBO;
    SetupSkeletalVAO(&VAO, &VBO, &EBO, skeletalVertices, indices);
    skeletons->skeletons_array[SkeletonTypes::avatar] = skeleton;
    skeletons->vao_array[SkeletonTypes::avatar] = VAO;
    skeletons->element_buffer_sizes[SkeletonTypes::avatar] = indices.size();

    // Create the shaders
    CreateSkinningProgram(&program->program_type.defaultProgram, &program->uniforms_type.skinningUniforms);

    Bind(program->program_type.defaultProgram.program);

    // Lastly, determine the position and rotation of the avatar

    SubInitializeUser(user);
}

void SubInitializeUser(UserSuper* user) {
    user->user_transform.rot = glm::vec3(degreesToRadians(-90), 0, 0);
    user->user_transform.scale = glm::vec3(100, 100, 100);
    user->user_transform.pos.z = 50;
    user->user_transform.pos.x = -WIDTH / 2;
    user->user_transform.pos.y = HEIGHT / 2;

    // The two key frames for avatar hold and animation end.
    user->animation_keys.state_1 = 3.6f;
    user->animation_keys.state_2 = 5.5f; // These values are hard-coded at the moment, meaning there must be a better way of figuring out the keyframe values.
    // ... and set the animation timer to 0 of course
    user->animation_timer = 0;
    user->stretch_booleans = { false, false };
}

void UpdateUser(UserSuper* user, View* view, float deltaTime) {
    SetPlayerInputVariables(&user->stretch_booleans, &user->rotation_incrementers);

    if (mouseWheel.input_received)
    {
        CameraSlide(&mouseWheel, view, &user->slide_props, deltaTime);
    }

    if (user->animation_timer == 0)
    {
        IncrementRotation(&user->direction, &user->user_transform.rot, user->rotation_incrementers, deltaTime);
    }

    if (user->stretch_booleans.pull_in || user->stretch_booleans.stretch_out)
    {
        Animate(deltaTime, &user->animation_timer, user->animation_keys, &user->stretch_booleans, &user->glide_variables);
    }

    // Commenting this for the sake of the ortographic perspective
    // SetCameraPos(&view->pos, user->user_transform.pos);

    mouseWheel.input_received = false;
}

void SetPlayerInputVariables(StretchBooleans* stretch, RotationIncrementers* rotInc) {
    // Start with rotation incrementers
    if (currentUpdate.D)
    {
        rotInc->increment_left = 12;
        rotInc->increment_right = 0;
    }
    else if (!currentUpdate.D && lastUpdate.D)
    {
        rotInc->increment_left = 0;
        rotInc->increment_right = 0;
    }

    if (currentUpdate.F)
    {
        rotInc->increment_left = 0;
        rotInc->increment_right = -12;
    }
    else if (!currentUpdate.F && lastUpdate.F)
    {
        rotInc->increment_left = 0;
        rotInc->increment_right = 0;
    }

    // Then the stretch booleans
    if (currentUpdate.LEFT_MOUSEBUTTON)
    {
        stretch->stretch_out = true;
    }
    else if (!currentUpdate.LEFT_MOUSEBUTTON && lastUpdate.LEFT_MOUSEBUTTON)
    {
        stretch->pull_in = true;
    }
}

void SetUserGraphicsData(ProgramSuper graphicsData, Camera camera, Transform userTransform, Skeleton skeleton) {
    UpdateSkinningUniforms(TransformMatrix(userTransform), camera, skeleton, graphicsData.uniforms_type.skinningUniforms);

    // For now we will use an arbitrary value for the light
    UpdateCameraLightBuffer(glm::vec3(0, 0, -8), camera.view.pos, graphicsData.program_type.defaultProgram.program);
}

void SetUserGraphicsDataOrtho(ProgramSuper graphicsData,
                              OrthoCamera camera,
                              Transform userTransform,
                              Skeleton skeleton) {
    UpdateSkinningUniformsOrtho(TransformMatrix(userTransform), camera, skeleton, graphicsData.uniforms_type.skinningUniforms);

    // Arbitrary value for the light
    UpdateCameraLightBuffer(glm::vec3(0, 0, -8), camera.view.pos, graphicsData.program_type.defaultProgram.program);
}
