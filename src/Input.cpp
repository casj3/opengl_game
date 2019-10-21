#include "Input.h"

#include <SDL.h>

void CheckInput(Input* currentUpdate, Input* lastUpdate, MouseWheel* mouseWheel)
{
	lastUpdate->D = currentUpdate->D;
	lastUpdate->F = currentUpdate->F;
	lastUpdate->ESCAPE = currentUpdate->ESCAPE;
	lastUpdate->LEFT_MOUSEBUTTON = currentUpdate->LEFT_MOUSEBUTTON;
	lastUpdate->RIGHT_MOUSEBUTTON = currentUpdate->RIGHT_MOUSEBUTTON;

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
						currentUpdate->D = true;
						break;
					case SDLK_f:
						currentUpdate->F = true;
						break;
					case SDLK_ESCAPE:
						currentUpdate->ESCAPE = true;
						break;
				}
				break;
			case SDL_KEYUP:
				switch (event.key.keysym.sym)
				{
					case SDLK_d:
						currentUpdate->D = false;
						break;
					case SDLK_f:
						currentUpdate->F = false;
						break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
					case SDL_BUTTON_LEFT:
						currentUpdate->LEFT_MOUSEBUTTON = true;
						break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
					case SDL_BUTTON_LEFT:
						currentUpdate->LEFT_MOUSEBUTTON = false;
						break;
					case SDL_BUTTON_RIGHT:
						currentUpdate->RIGHT_MOUSEBUTTON = false;
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
				currentUpdate->ESCAPE = true;
				break;
		}
	}
}
