#include "input.h"

#include <SDL.h>

void CheckInput(Input* currentUpdate, Input* lastUpdate, Mouse_Wheel* mouseWheel)
{
    lastUpdate->d = currentUpdate->d;
    lastUpdate->f = currentUpdate->f;
    lastUpdate->esc = currentUpdate->esc;
    lastUpdate->left_mouse_btn = currentUpdate->left_mouse_btn;
    lastUpdate->right_mouse_btn = currentUpdate->right_mouse_btn;

    mouseWheel->input_received = false;

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_d:
                currentUpdate->d = true;
                break;
            case SDLK_f:
                currentUpdate->f = true;
                break;
            case SDLK_ESCAPE:
                currentUpdate->esc = true;
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.sym)
            {
            case SDLK_d:
                currentUpdate->d = false;
                break;
            case SDLK_f:
                currentUpdate->f = false;
                break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT:
                currentUpdate->left_mouse_btn = true;
                break;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT:
                currentUpdate->left_mouse_btn = false;
                break;
            case SDL_BUTTON_RIGHT:
                currentUpdate->right_mouse_btn = false;
                break;
            }
            break;

        case SDL_MOUSEWHEEL:
            if (event.wheel.y == -1)
            {
                mouseWheel->down = true;
                mouseWheel->up = false;
            }
            else if (event.wheel.y == 1)
            {
                mouseWheel->down = false;
                mouseWheel->up = true;
            }

            mouseWheel->input_received = true;
            break;
        case SDL_QUIT:
            currentUpdate->esc = true;
            break;
        }
    }
}
