#include <SDL.h>
#include <SDL_image.h>
#include <math.h>
#include <string>
#include <stdio.h>
#include <iostream>

#include "GameRenderer.h"
#include "ETexture.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PIXEL_MULT = 2;


SDL_Window* SDLWindow = nullptr; //The window that will be rendered to
SDL_Renderer* SDLRenderer = nullptr; //The window renderer
ETexture tileset;
ETexture fontTex;

bool InitRenderer();
bool LoadMedia();
void CloseRenderer();

GameRenderer::GameRenderer() {
	InitRenderer();
}

GameRenderer::~GameRenderer() {
	CloseRenderer();
}

void GameRenderer::Render() {
	SDL_SetRenderDrawColor(SDLRenderer, 31, 14, 28, 0x00);
	SDL_RenderClear(SDLRenderer);

	//render stuff here

	SDL_RenderPresent(SDLRenderer);
}

bool InitRenderer() {
	bool success = true;

	//InitRenderer SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0){
		printf("SDL could not initialize! %s\n", SDL_GetError());
		success = false;
	}
	else{
		//Set texture filtering to linear (keeps pixel art crisp)
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0")){
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		SDLWindow = SDL_CreateWindow("Roguelike Rework", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (!SDLWindow){
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else{
			//Create renderer for window
			SDLRenderer = SDL_CreateRenderer(SDLWindow, -1, SDL_RENDERER_ACCELERATED);
			if (SDLRenderer == nullptr){
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}else{
				//InitRenderer renderer color
				SDL_SetRenderDrawColor(SDLRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//InitRenderer PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags)){
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
		if (!LoadMedia()) {
			success = false;
		}
	}
	return success;
}

bool LoadMedia() {
	//Loading success flag
	bool success = true;

	if (tileset.LoadFromFile("tilesets/tileset.png", SDLRenderer)) {
		tileset.SetTileSetInfo(16, 16);
	}
	if (fontTex.LoadFromFile("fonts/font.png", SDLRenderer)) {
		fontTex.SetTileSetInfo(8, 12);
	}
	else {
		success = false;
	}

	if (!success) {
		printf("Failed to load media!\n");
	}
	return success;
}

void CloseRenderer()
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