#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <iostream>
#include <string>

#include "ETexture.h"
#include "GameLoop.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_FPS = 60;
const int UPDATES_PER_SECOND = 4;
const int MS_PER_FRAME = 1000 / SCREEN_FPS;
const int MS_PER_UPDATE = 1000 / UPDATES_PER_SECOND;

//Starts up SDL and creates window
bool Init();

//Loads media
bool LoadMedia();

//Frees media and shuts down SDL
void Close();

void Render();
void AdvanceGameLogic();

//The window we'll be rendering to
SDL_Window* SDLWindow = NULL;

//The window renderer
SDL_Renderer* SDLRenderer = NULL;

//Tileset texture:
ETexture tileTest;

//Game loop handler
GameLoop* gameLoop = NULL;

bool Init()
{
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		SDLWindow = SDL_CreateWindow("SDL Application", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (SDLWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			SDLRenderer = SDL_CreateRenderer(SDLWindow, -1, SDL_RENDERER_ACCELERATED);
			if (SDLRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(SDLRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}

	gameLoop = new GameLoop();

	if (!success) {
		printf("Failed to initialize!\n");
	}
	return success;
}

bool LoadMedia()
{
	//Loading success flag
	bool success = true;

	if (tileTest.LoadFromFile("tileTest.png", SDLRenderer)) {
		tileTest.SetTileSetInfo(16, 16);
	}
	else {
		success = false;
	}

	if (!success) {
		printf("Failed to load media!\n");
	}
	return success;
}

void Close()
{
	//Destroy window	
	SDL_DestroyRenderer(SDLRenderer);
	SDL_DestroyWindow(SDLWindow);
	SDLWindow = NULL;
	SDLRenderer = NULL;
	tileTest.Free();

	//Free up game systems
	delete gameLoop;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
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
					AdvanceGameLogic();
				}
				if (currentTime >= nextRenderTime) {
					nextRenderTime += MS_PER_FRAME;
					Render();
				}
			}
		}
	}
	Close(); //Free resources and close SDL
	return 0;
}

void Render() {
	SDL_SetRenderDrawColor(SDLRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(SDLRenderer);

	SDL_Rect renderQuad = { 32, 32, 128, 128 };
	SDL_Rect texQuad = *tileTest.GetTileRect(33);
	SDL_RenderCopy(SDLRenderer, tileTest.GetTexture(), &texQuad, &renderQuad);

	//Update screen
	SDL_RenderPresent(SDLRenderer);
}

void AdvanceGameLogic() {
	gameLoop->AdvanceLoop();
}