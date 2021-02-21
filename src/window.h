#pragma once

#define WIN_WIDTH 1280.f
#define WIN_HEIGHT 720.f

void InitializeDisplay(const char* title);
void UpdateDisplay();
void ClearDisplay(float r, float g, float b, float a);
void DestroyDisplay();
