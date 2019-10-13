#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <iostream>
#include <string>

#include "ETexture.h"
#include "GameLoop.h"
#include "GameRenderer.h"

//Screen constants
const int SCREEN_FPS = 1;
const int UPDATES_PER_SECOND = 4;
const int MS_PER_FRAME = 1000 / SCREEN_FPS;
const int MS_PER_UPDATE = 1000 / UPDATES_PER_SECOND;

//Starts up SDL and creates window
bool Init();
bool LoadMedia();
void Close();

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
	if (!gameRenderer->Init()) {
		success = false;
	}
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

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (Init())
	{
		if (LoadMedia())
		{
			bool quit = false; //Main loop flag
			SDL_Event e; //Event handler

			Uint32 currentTime = SDL_GetTicks();
			Uint32 nextGameUpdateTime = SDL_GetTicks();
			Uint32 nextRenderTime = SDL_GetTicks();

			int framesCounted = 0;
			int gameUpdates = 0;

			while (!quit) {
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0){
					//User requests quit
					if (e.type == SDL_QUIT){
						quit = true;
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
	}
	Close(); //Free resources and close SDL
	return 0;
}