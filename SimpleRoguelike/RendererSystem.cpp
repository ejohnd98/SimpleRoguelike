#include <stdio.h>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <math.h>

#include "RendererSystem.h"
#include "ETexture.h"
#include "ECS.h"

extern ECS ecs;

const char* WINDOW_TITLE = "Roguelike Rework";
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PIXEL_MULT = 2;

SDL_Window* SDLWindow = nullptr; //The window that will be rendered to
SDL_Renderer* SDLRenderer = nullptr; //The window renderer
ETexture tileset;
ETexture fontTex;

void RendererSystem::Init() {
	bool success = true;

	//initialize SDL renderer
	assert(SDL_Init(SDL_INIT_VIDEO) >= 0);

	//Set texture filtering to linear (keeps pixel art crisp)
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0")) {
		printf("Warning: Linear texture filtering not enabled!");
	}

	//Create window
	SDLWindow = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	assert(SDLWindow);

	//Create renderer for window
	SDLRenderer = SDL_CreateRenderer(SDLWindow, -1, SDL_RENDERER_ACCELERATED);
	assert(SDLRenderer != nullptr);

	//Set renderer color
	SDL_SetRenderDrawColor(SDLRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

	//InitRenderer PNG loading
	int imgFlags = IMG_INIT_PNG;
	assert(IMG_Init(imgFlags) & imgFlags);

	assert(LoadMedia());
}

void RendererSystem::Close()
{
	//Destroy window	
	SDL_DestroyRenderer(SDLRenderer);
	SDL_DestroyWindow(SDLWindow);
	SDLWindow = nullptr;
	SDLRenderer = nullptr;
	tileset.Free();

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

void RendererSystem::Render() {
	SDL_SetRenderDrawColor(SDLRenderer, 31, 14, 28, 0x00);
	SDL_RenderClear(SDLRenderer);

	//render stuff

	SDL_RenderPresent(SDLRenderer);
}

bool RendererSystem::LoadMedia() {
	//Loading success flag
	bool success = true;

	if (tileset.LoadFromFile("tilesets/tileset.png", SDLRenderer)) {
		tileset.SetTileSetInfo(16, 16);
	}else {
		success = false;
	}

	if (fontTex.LoadFromFile("fonts/font.png", SDLRenderer)) {
		fontTex.SetTileSetInfo(8, 12);
	}else {
		success = false;
	}
	return success;
}