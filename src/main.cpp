#define STB_IMAGE_IMPLEMENTATION

#include <SDL.h>

#include "window.h"
#include "camera.h"
#include "gpu_program.h"
#include "player.h"
#include "animate.h"
#include "input.h"
#include "scene.h"
#include "enums.h"
#include "utils/array_pool.h"
#include "utils/print.h"

#define NUM_POOLS 2

int main(int argc, char** argv)
{
    // The display and openGL are instantiated
    InitializeDisplay("Data Oriented Attempt");
    InitArrayPools(NUM_POOLS);

    // To calculate the time that passes every frame, e.g. deltaTime
    float now = SDL_GetTicks();
    float last = 0;
    float delta_time = 0;

    // The means by which a camera is constructed
    Camera camera;

    // Containers of program data used to create programs constituted of shaders
    Program_Super programs[Program_Types::NUM_PROGRAMS];

    // Draw units are grouped by their use of a certain draw function and there are different kind of draw
    //units depending on the materials and textures they use. This one uses textures.
    Draw_Units_Textured skeletal_draw_units;

    // The user super structure
    User_Super user;
    user.animation_timer = 0;

    ARRAY_HANDLE(struct Skeleton) skeletonArray = GET_ARRAY_HANDLE(struct Skeleton, Skeleton_Types::NUM_SKELETONS);
    ARRAY_HANDLE(uint32_t) vaoArray = GET_ARRAY_HANDLE(uint32_t, Skeleton_Types::NUM_SKELETONS);
    ARRAY_HANDLE(uint32_t) elementBufferSizes = GET_ARRAY_HANDLE(uint32_t, Skeleton_Types::NUM_SKELETONS);
    struct Skeletons skeletons = { skeletonArray, vaoArray, elementBufferSizes };

    // The starting state is set
    State state = State::INIT_SCENE_1;

    while (!current_update.esc)
    {
        ClearDisplay(0.3f, 0.3f, 0.3f, 0.3f);

        // The time in seconds passed every frame is calculated
        last = now;
        now = SDL_GetTicks();
        delta_time = (now - last) / 1000;

        // Our input units need to be assigned values every loop
        CheckInput(&current_update, &last_update, &mouse_wheel);

        switch (state) {
        case State::INIT_SCENE_1: {
            unsigned int before = SDL_GetTicks();
            InitializeScene1(&programs[Program_Types::SKINNING], &skeletons, &user);
            unsigned int after = SDL_GetTicks();
            unsigned int init_delta = after - before;
            Print("%u to initialize scene\n", init_delta);
            // All is well for the first scene to start
            state = State::SCENE_1;

            break;
        }
        case State::SCENE_1: {
            UpdateUser(&user, &camera.view, delta_time);
            AnimateBones(user.animation_timer, *skeletons.skeletons_array[Skeleton_Types::AVATAR]);
            SetUserGraphicsData(programs[Program_Types::SKINNING],
                                camera,
                                user.user_transform,
                                *skeletons.skeletons_array[Skeleton_Types::AVATAR]);
            // Arbitrary phong material
            Phong_Material avatarPhong = {
                { 0.1f, 0.1f, 0.0f },
                { 0.5f, 0.5f, 0.0f },
                { 1, 1, 1 },
                1000
            };
            uint32_t program = programs[Program_Types::SKINNING].program_type.default_program.program;
            DrawPhongVAO(program,
                         *skeletons.vao_array[Skeleton_Types::AVATAR],
                         *skeletons.element_buffer_sizes[Skeleton_Types::AVATAR],
                         avatarPhong);
            break;
        }
        }

        UpdateDisplay();
    }

    return 0;
}
