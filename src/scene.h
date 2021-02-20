#pragma once

#include <stdint.h>

#include "player.h"
#include "draw.h"
#include "gpu_program.h"
#include "load.h"
#include "animate.h"

void InitializeScene1(ProgramSuper* program, struct Skeletons* skeletons, UserSuper* user);

void UpdateUser(UserSuper* user, View* view, float deltaTime);

void SetPlayerInputVariables(StretchBooleans* stretch, RotationIncrementers* rotInc);

void SetUserGraphicsData(ProgramSuper graphicsData, Camera camera, Transform userTransform, Skeleton bones);

void SetUserGraphicsDataOrtho(ProgramSuper graphicsData, OrthoCamera camera, Transform userTransform, Skeleton bones);

void SubInitializeUser(UserSuper* user);
