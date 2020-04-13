#include <SDL.h>
#include <stdio.h>
#include <iostream>

#include "Game.h"
#include "GameRenderer.h"
#include "Commands.h"

//Screen constants
const int SCREEN_FPS = 60;
const int UPDATES_PER_SECOND = 6;
const int MS_PER_FRAME = 1000 / SCREEN_FPS;
const int MS_PER_UPDATE = 1000 / UPDATES_PER_SECOND;

//Functions
bool Initialize();
void Terminate();
Command InputToCommand(SDL_Event* e);

//Variables
std::shared_ptr<Game> game;
std::shared_ptr<GameRenderer> gameRenderer;

bool Initialize()
{
	bool success = true;

	game = std::make_shared<Game>();

	gameRenderer = std::make_shared<GameRenderer>();

	if (!game || !gameRenderer) {
		printf("ERROR: Failed to initialize!\n");
	}
	return success;
}

void Terminate()
{
}

Command InputToCommand(SDL_Event* e) { //hardcoded inputs currently
	switch (e->key.keysym.sym) {
		case SDLK_UP:
			return Command::MOVE_UP;
		case SDLK_DOWN:
			return Command::MOVE_DOWN;
		case SDLK_RIGHT:
			return Command::MOVE_RIGHT;
		case SDLK_LEFT:
			return Command::MOVE_LEFT;
		case SDLK_SPACE:
			return Command::WAIT;
	}
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (Initialize())
	{
		bool quit = false; //Main loop flag

		Uint32 currentTime = SDL_GetTicks();
		Uint32 nextGameUpdateTime = SDL_GetTicks();
		Uint32 nextRenderTime = SDL_GetTicks() + MS_PER_FRAME;

		SDL_Event event; //Event handler

		while (!quit) {
			//Handle events on queue
			while (SDL_PollEvent(&event) != 0){
				switch (event.type) {
					case SDL_KEYDOWN:
						game->GiveInput(InputToCommand(&event));
						break;
					case SDL_QUIT:
						quit = true;
						break;
				}
			}
			currentTime = SDL_GetTicks();

			if (currentTime >= nextGameUpdateTime) {
				nextGameUpdateTime += MS_PER_UPDATE;
				int diff = ((int)nextGameUpdateTime) - ((int)currentTime);
				//std::cout << "Advancing loop at: " << currentTime << " Next: " << nextGameUpdateTime << " Diff: " << diff << "\n";
				game->Advance();
			}
			if (currentTime >= nextRenderTime) {
				nextRenderTime += MS_PER_FRAME;
				gameRenderer->Render();
			}
		}
	}
	Terminate(); //Free resources and close SDL
	return 0;
}