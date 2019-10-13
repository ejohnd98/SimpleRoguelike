#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <iostream>
#include <string>

#include "GameLoop.h"
#include "GameRenderer.h"
#include "Commands.h"

//Screen constants
const int SCREEN_FPS = 60;
const int UPDATES_PER_SECOND = 15;
const int MS_PER_FRAME = 1000 / SCREEN_FPS;
const int MS_PER_UPDATE = 1000 / UPDATES_PER_SECOND;

//Starts up SDL and creates window
bool Init();
void Close();
Command InputToCommand(SDL_Event* e);

//Game loop handler
GameLoop* gameLoop = nullptr;
//Game Renderer
GameRenderer* gameRenderer = nullptr;

bool Init()
{
	bool success = true;

	gameLoop = new GameLoop();
	gameRenderer = new GameRenderer();
	gameRenderer->SetMap(gameLoop->GetMapPointer());
	if (!success) {
		printf("Failed to initialize!\n");
	}
	return success;
}

void Close()
{
	//Free up game systems
	delete gameLoop;
	delete gameRenderer;
}

Command InputToCommand(SDL_Event* e) {
	switch (e->key.keysym.sym) {
		case SDLK_UP:
			return Command::MOVE_UP;
		case SDLK_DOWN:
			return Command::MOVE_DOWN;
		case SDLK_RIGHT:
			return Command::MOVE_RIGHT;
		case SDLK_LEFT:
			return Command::MOVE_LEFT;
	}
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (Init())
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
						gameLoop->GiveInput(InputToCommand(&event));
						break;
					case SDL_QUIT:
						quit = true;
						break;
				}
			}
			currentTime = SDL_GetTicks();

			if (currentTime >= nextGameUpdateTime) {
				nextGameUpdateTime += MS_PER_UPDATE;
				gameLoop->AdvanceLoop();
			}
			if (currentTime >= nextRenderTime) {
				nextRenderTime += MS_PER_FRAME;
				gameRenderer->Render();
			}
		}
	}
	Close(); //Free resources and close SDL
	return 0;
}