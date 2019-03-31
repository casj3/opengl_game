#define STB_IMAGE_IMPLEMENTATION

#include "Display.h"
#include "LoopManagement.h"
#include "GeometricFigures.h"
#include "enums.h"

int main(int argc, char** argv)
{
	// The display and openGL are instantiated
	InitializeDisplay(WIDTH, HEIGHT, "Data Oriented Attempt");

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

	// This vector contains all skeletons and each skeleton consists of a set of bones which in turn posesses its transforms throughout time -
	// each transform corresponding to a key in time and each key being represented by an index in the vector
	SkeletonSuper skeletons[num_skeletons];

	// The surfaces used in the game
	//vector<Quad> surfaces;

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
				InitializeScene1(&skeletalDrawUnits, &programs[skinning], &skeletons[avatar], &user);

				// All is well for the first scene to start
				state = scene1;

				break;
			}
			case scene1:
			{
				UpdateUser(&user, &camera.view, delta_time);
				SetAnimationBones(user.animation_timer, &skeletons[avatar]);
				SetUserGraphicsDataOrtho(programs[skinning], camera, user.user_transform, skeletons[avatar]);
				DrawSkeletalDrawUnits(&skeletalDrawUnits, programs[skinning].program_type.defaultProgram.program);
				break;
			}
		}

		UpdateDisplay();
	}

	DestroyDisplay();

	// Destroy programs
	for (uint i = 0; i < num_programs; i++)
	{
		DestroyDefaultProgram(programs[i].program_type.defaultProgram);
	}
	// Destroy skeletons
	for (uint i = 0; i < num_skeletons; i++)
	{
		DestroySkeleton(skeletons[i].animation_bone_transforms);
	}
	// Iterate to destroy the draw units
	for (uint i = 0; i < skeletalDrawUnits.vertex_array_objects.size(); i++)
	{
		DestroyVAO(&skeletalDrawUnits.vertex_array_objects[i]);
		DestroyTextures(skeletalDrawUnits.textures[i]);
	}

	return 0;
}
