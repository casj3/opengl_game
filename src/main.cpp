#define STB_IMAGE_IMPLEMENTATION

#include <SDL.h>

#include "Display.h"
#include "Camera.h"
#include "Shader.h"
#include "User.h"
#include "Animation.h"
#include "Input.h"
#include "LoopManagement.h"
#include "enums.h"
#include "utils/ArrayPool.h"

// TODO: If we let the system dynamically allocate and reallocate the whole system breaks. Look at a solution.
#define START_CAPACITY 200

int main(int argc, char** argv)
{
    // The display and openGL are instantiated
    InitializeDisplay(WIDTH, HEIGHT, "Data Oriented Attempt");
    InitArrayPool(START_CAPACITY);

    // To calculate the time that passes every frame, e.g. deltaTime
    float now = SDL_GetTicks();
    float last = 0;
    float delta_time = 0;

    // The means by which a camera is constructed
    Camera camera;

    // Containers of program data used to create programs constituted of shaders
    ProgramSuper programs[ProgramTypes::num_programs];

    // Draw units are grouped by their use of a certain draw function and there are different kind of draw
    //units depending on the materials and textures they use. This one uses textures.
    DrawUnitsTextured skeletalDrawUnits;

    // The user super structure
    UserSuper user;
    user.animation_timer = 0;

    ARRAY_HANDLE(struct Skeleton) skeletonArray = GET_ARRAY_HANDLE(struct Skeleton, SkeletonTypes::num_skeletons);
    ARRAY_HANDLE(uint32_t) vaoArray = GET_ARRAY_HANDLE(uint32_t, SkeletonTypes::num_skeletons);
    ARRAY_HANDLE(uint32_t) elementBufferSizes = GET_ARRAY_HANDLE(uint32_t, SkeletonTypes::num_skeletons);
    struct Skeletons skeletons = { skeletonArray, vaoArray, elementBufferSizes };

    // The starting state is set
    State state = initializeScene1;

    while (!currentUpdate.ESCAPE)
    {
        ClearDisplay(0.3f, 0.3f, 0.3f, 0.3f);

        // The time in seconds passed every frame is calculated
        last = now;
        now = SDL_GetTicks();
        delta_time = (now - last) / 1000;

        // Our input units need to be assigned values every loop
        CheckInput(&currentUpdate, &lastUpdate, &mouseWheel);

        switch (state) {
        case initializeScene1:
            InitializeScene1(&programs[skinning], &skeletons, &user);

            // All is well for the first scene to start
            state = scene1;

            break;
        case scene1:
            UpdateUser(&user, &camera.view, delta_time);
            AnimateBones(user.animation_timer, *skeletons.skeletons_array[avatar]);
            SetUserGraphicsData(programs[skinning], camera, user.user_transform, *skeletons.skeletons_array[avatar]);
            // Arbitrary phong material
            PhongMaterial avatarPhong = {
                { 0.1f, 0.1f, 0.0f },
                { 0.5f, 0.5f, 0.0f },
                { 1, 1, 1 },
                1000
            };
            uint32_t program = programs[skinning].program_type.defaultProgram.program;
            DrawPhongVAO(program, *skeletons.vao_array[avatar], *skeletons.element_buffer_sizes[avatar], avatarPhong);
            break;
        }

        UpdateDisplay();
    }

    return 0;
}
