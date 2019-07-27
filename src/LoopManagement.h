#pragma once

#include "User.h"
#include "Draw_VAO.h"
#include "Shader.h"
#include "LoadVAO.h"
#include "Animation.h"

void InitializeScene1(DrawUnitsTextured* skeletalDrawUnits, ProgramSuper* program, SkeletonSuper* skeletonSuper, UserSuper* user);

void UpdateUser(UserSuper* user, View* view, float deltaTime);

void SetPlayerInputVariables(StretchBooleans* stretch, RotationIncrementers* rotInc);

void SetUserGraphicsData(ProgramSuper graphicsData, Camera camera, Transform userTransform, SkeletonSuper bones);

void SetUserGraphicsDataOrtho(ProgramSuper graphicsData, OrthoCamera camera, Transform userTransform, SkeletonSuper bones);

void DrawSkeletalDrawUnits(DrawUnitsTextured* skeletalDrawUnits, unsigned int program);

void SubInitializeUser(UserSuper* user);
