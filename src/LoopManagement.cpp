#include "LoopManagement.h"

#include <vector>

#include <glm/glm.hpp>

#include "utils/MathFunctions.h"
#include "VAO_Data.h"

// Global variables which are declared as extern in different header files
Input currentUpdate;
Input lastUpdate;
MouseWheel mouseWheel;

void InitializeScene1(DrawUnitsTextured* skeletalDrawUnits, ProgramSuper* program,
                      SkeletonSuper* skeletonSuper, UserSuper* user)
{
	// An importer to import ...
	Assimp::Importer importer;
	std::string avatarPath = "./models/test.fbx";
	// ... a scene
	const aiScene* scene = ImportScene(&importer, avatarPath);

	// All the vertices and indices to create a VAO
  std::vector<SkeletalVertex> skeletalVertices = LoadSkeletalVertices(scene->mMeshes[0], &skeletonSuper->key_bone_frames, scene);
  std::vector<unsigned int> indices = LoadIndices(scene->mMeshes[0]);

	// At the moment I don't think I need to store the data of the VBO and EBO,
  // which may turn out to be a grave error
	unsigned int VAO, VBO, EBO;
	SetupSkeletalVAO(&VAO, &VBO, &EBO, skeletalVertices, indices);
	Textures textures = LoadMeshTextures(scene->mMeshes[0], scene, avatarPath);

	// Allocate space for the skeleton whose data is sent to the GPU
	skeletonSuper->animation_bone_transforms =
    (glm::mat4*)malloc(sizeof(glm::mat4) * skeletonSuper->key_bone_frames.size());
	// Now the size of the array
	skeletonSuper->bone_amount = (sizeof(glm::mat4)* skeletonSuper->key_bone_frames.size()) / sizeof(glm::mat4);

	// It's extremely important that these two push_back operations
  // happen after each other, such that both vectors are parallel, matching
	skeletalDrawUnits->vertex_array_objects.push_back(VAO);
	skeletalDrawUnits->textures.push_back(textures);
	skeletalDrawUnits->indices_size.push_back(indices.size());

	// Create the shaders
	CreateSkinningProgram(&program->program_type.defaultProgram, &program->uniforms_type.skinningUniforms);

	Bind(program->program_type.defaultProgram.program);

	// Lastly, determine the position and rotation of the avatar

	SubInitializeUser(user);
}

void SubInitializeUser(UserSuper* user)
{
	user->user_transform.rot = glm::vec3(degreesToRadians(-90), 0, 0);
	user->user_transform.scale = glm::vec3(100, 100, 100);
	user->user_transform.pos.z = 50;
	user->user_transform.pos.x = -WIDTH/2;
	user->user_transform.pos.y = HEIGHT / 2;

	// ... and the time for the two key frames essential for gameplay
	user->animation_keys.state_1 = 3.6f;
	user->animation_keys.state_2 = 5.5f; // These values are hard-coded at the moment, meaning there must be a better way of figuring out the keyframe values.
	// ... and set the animation timer to 0 of course
	user->animation_timer = 0;
	user->stretch_booleans = { false, false };
}

void UpdateUser(UserSuper* user, View* view, float deltaTime)
{
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
	//SetCameraPos(&view->pos, user->user_transform.pos);

	mouseWheel.input_received = false;
}

void SetPlayerInputVariables(StretchBooleans* stretch, RotationIncrementers* rotInc)
{
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

void SetUserGraphicsData(ProgramSuper graphicsData, Camera camera, Transform userTransform, SkeletonSuper skeleton)
{
	UpdateSkinningUniforms(TransformMatrix(userTransform), camera, skeleton, graphicsData.uniforms_type.skinningUniforms);

	// For now we will use an arbitrary value for the light
	UpdateCameraLightBuffer(glm::vec3(0, 0, -8), camera.view.pos, graphicsData.program_type.defaultProgram.program);
}

void SetUserGraphicsDataOrtho(ProgramSuper graphicsData,
                              OrthoCamera camera,
                              Transform userTransform,
                              SkeletonSuper skeleton)
{
	UpdateSkinningUniformsOrtho(TransformMatrix(userTransform), camera, skeleton, graphicsData.uniforms_type.skinningUniforms);

	// For now we will use an arbitrary value for the light
	UpdateCameraLightBuffer(glm::vec3(0, 0, -8), camera.view.pos, graphicsData.program_type.defaultProgram.program);
}

void DrawSkeletalDrawUnits(DrawUnitsTextured* skeletalDrawUnits, unsigned int program)
{
	// We choose an arbitrary phong material for the avatar draw unit

	PhongMaterial avatarPhong =
	{
		{ 0.1f, 0.1f, 0.0f },
		{ 0.5f, 0.5f, 0.0f },
		{ 1, 1, 1 },
		1000
	};

	// We draw the avatar without the texture to test the validity of the shaders. OBS! This function is clearly not
	// finished for the sake of scalability.

	for (unsigned int i = 0; i < skeletalDrawUnits->vertex_array_objects.size(); i++)
	{
		DrawPhongVAO(program, skeletalDrawUnits->vertex_array_objects[i], skeletalDrawUnits->indices_size[i], avatarPhong);
	}
}
