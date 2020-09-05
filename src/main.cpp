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
#include "utils/ArrayManager.h"

#define ALLOC_START_CAPACITY 10

int main(int argc, char** argv)
{
    // The display and openGL are instantiated
    InitializeDisplay(WIDTH, HEIGHT, "Data Oriented Attempt");
    InitArrayManager(ALLOC_START_CAPACITY);

    // To calculate the time that passes every frame, e.g. deltaTime
    float now = SDL_GetTicks();
    float last = 0;
    float delta_time = 0;

    // The means by which a camera is constructed
    OrthoCamera camera;

    // Containers of program data used to create programs constituted of shaders
    ProgramSuper programs[num_programs];

    // Draw units are grouped by their use of a certain draw function and there are different kind of draw
    //units depending on the materials and textures they use. This one uses textures.
    DrawUnitsTextured skeletalDrawUnits;

    // The user super structure
    UserSuper user;
    user.animation_timer = 0;

    struct Skeletons skeletons;

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

        switch (state)
        {
        case initializeScene1:
        {
            InitializeScene1(&skeletalDrawUnits, &programs[skinning], &skeletons, &user);

            // All is well for the first scene to start
            state = scene1;

            break;
        }
        // case scene1:
        // {
        //     UpdateUser(&user, &camera.view, delta_time);
        //     SetAnimationBones(user.animation_timer, nullptr);
        //     SetUserGraphicsDataOrtho(programs[skinning], camera, user.user_transform, nullptr);
        //     DrawSkeletalDrawUnits(&skeletalDrawUnits, programs[skinning].program_type.defaultProgram.program);
        //     break;
        // }
        }

        UpdateDisplay();
    }

    return 0;
}
