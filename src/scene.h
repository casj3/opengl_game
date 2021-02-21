#pragma once

#include <stdint.h>

#include "player.h"
#include "draw.h"
#include "gpu_program.h"
#include "load.h"
#include "animate.h"

void InitializeScene1(Program_Super* program, struct Skeletons* skeletons, User_Super* user);

void UpdateUser(User_Super* user, View* view, float deltaTime);

void SetPlayerInputVariables(Stretch_Booleans* stretch, Rotation_Incrementers* rotInc);

void SetUserGraphicsData(Program_Super graphicsData, Camera camera, Transform userTransform, Skeleton bones);

void SetUserGraphicsDataOrtho(Program_Super graphicsData, Ortho_Camera camera, Transform userTransform, Skeleton bones);

void SubInitializeUser(User_Super* user);
