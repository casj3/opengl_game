#include "scene.h"

#include <vector>

#include <glm/glm.hpp>

#include <utils/array_pool.h>
#include "utils/math_functions.h"
#include "gpu_store.h"
#include "enums.h"

// Global variables which are declared as extern in different header files
Input current_update;
Input last_update;
Mouse_Wheel mouse_wheel;
Assimp::Importer importer;

// TODO: Can't load scenes like this.
void InitializeScene1(Program_Super* program, struct Skeletons* skeletons, User_Super* user) {
    std::string avatarPath = "./assets/models/Duck_Running.fbx";
    const aiScene* scene = ImportScene(&importer, avatarPath);

    // All the vertices and indices to create a VAO
    ARRAY_HANDLE(Skeletal_Vertex) skeletal_vertices;
    Skeleton skeleton = LoadSkeleton(scene, &skeletal_vertices);
    std::vector<uint32_t> indices = LoadIndices(scene->mMeshes[0]);

    // TODO: Revise what to do with the VBO and EBO.
    uint32_t VAO, VBO, EBO;
    SetupSkeletalVAO(&VAO, &VBO, &EBO, skeletal_vertices, indices);
    *skeletons->skeletons_array[Skeleton_Types::AVATAR] = skeleton;
    *skeletons->vao_array[Skeleton_Types::AVATAR] = VAO;
    *skeletons->element_buffer_sizes[Skeleton_Types::AVATAR] = indices.size();

    // Create the shaders
    CreateSkinningProgram(&program->program_type.default_program, &program->uniforms_type.skinning_uniforms);

    Bind(program->program_type.default_program.program);

    // Lastly, determine the position and rotation of the avatar

    SubInitializeUser(user);
}

void SubInitializeUser(User_Super* user) {
    user->user_transform.rot = glm::vec3(degreesToRadians(-90), 0, 0);
    user->user_transform.scale = glm::vec3(1, 1, 1);
    user->user_transform.pos.z = 50;
    user->user_transform.pos.x = -WIN_WIDTH / 2;
    user->user_transform.pos.y = WIN_HEIGHT / 2;

    // The two key frames for avatar hold and animation end.
    user->animation_keys.state_1 = 3.6f;
    user->animation_keys.state_2 = 5.5f; // These values are hard-coded at the moment, meaning there must be a better way of figuring out the keyframe values.
    // ... and set the animation timer to 0 of course
    user->animation_timer = 0;
    user->stretch_booleans = { false, false };
}

void UpdateUser(User_Super* user, View* view, float deltaTime) {
    SetPlayerInputVariables(&user->stretch_booleans, &user->rotation_incrementers);

    if (mouse_wheel.input_received)
    {
        CameraSlide(&mouse_wheel, view, &user->slide_props, deltaTime);
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
    SetCameraPos(&view->pos, user->user_transform.pos);

    mouse_wheel.input_received = false;
}

void SetPlayerInputVariables(Stretch_Booleans* stretch, Rotation_Incrementers* rotInc) {
    // Start with rotation incrementers
    if (current_update.d)
    {
        rotInc->increment_left = 12;
        rotInc->increment_right = 0;
    }
    else if (!current_update.d && last_update.d)
    {
        rotInc->increment_left = 0;
        rotInc->increment_right = 0;
    }

    if (current_update.f)
    {
        rotInc->increment_left = 0;
        rotInc->increment_right = -12;
    }
    else if (!current_update.f && last_update.f)
    {
        rotInc->increment_left = 0;
        rotInc->increment_right = 0;
    }

    // Then the stretch booleans
    if (current_update.left_mouse_btn)
    {
        stretch->stretch_out = true;
    }
    else if (!current_update.left_mouse_btn && last_update.left_mouse_btn)
    {
        stretch->pull_in = true;
    }
}

void SetUserGraphicsData(Program_Super graphicsData, Camera camera, Transform userTransform, Skeleton skeleton) {
    UpdateSkinningUniforms(TransformMatrix(userTransform), camera, skeleton, graphicsData.uniforms_type.skinning_uniforms);

    // For now we will use an arbitrary value for the light
    UpdateCameraLightBuffer(glm::vec3(0, 0, -8), camera.view.pos, graphicsData.program_type.default_program.program);
}

void SetUserGraphicsDataOrtho(Program_Super graphicsData,
                              Ortho_Camera camera,
                              Transform userTransform,
                              Skeleton skeleton) {
    UpdateSkinningUniformsOrtho(TransformMatrix(userTransform), camera, skeleton, graphicsData.uniforms_type.skinning_uniforms);

    // Arbitrary value for the light
    UpdateCameraLightBuffer(glm::vec3(0, 0, -8), camera.view.pos, graphicsData.program_type.default_program.program);
}
