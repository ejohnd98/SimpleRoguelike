#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <iostream>

#include "GameRenderer.h"
#include "ETexture.h"
#include "Map.h"
#include "Actor.h"
#include "Sprite.h"
#include "Prop.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int TILE_SIZE = 32;

//The window we'll be rendering to
SDL_Window* SDLWindow = nullptr;

//The window renderer
SDL_Renderer* SDLRenderer = nullptr;

//Tileset texture:
ETexture tileTest;

Map** mapPntr = nullptr;

bool InitRenderer();
bool LoadMedia();
void CloseRenderer();
void RenderMap(Map* map);

GameRenderer::GameRenderer() {
	InitRenderer();
}

GameRenderer::~GameRenderer() {
	CloseRenderer();
}

void GameRenderer::Render() {
	if (*mapPntr != nullptr) {
		RenderMap(*mapPntr);
	}
}
void GameRenderer::SetMap(Map** map) {
	mapPntr = map;
}

void RenderMap(Map* map) {
	SDL_SetRenderDrawColor(SDLRenderer, 31, 14, 28, 0x00);
	SDL_RenderClear(SDLRenderer);

	int height = map->GetHeight();
	int width = map->GetWidth();

	for (int y = 0;  y < height; y++) {
		for (int x = 0; x < width; x++) {
			Sprite* spr = map->GetCell(x, y)->GetSprite();
			SDL_Rect texQuad = *tileTest.GetTileRect(spr->GetIndex());
			
			SDL_Rect renderQuad = { x* TILE_SIZE, y* TILE_SIZE, TILE_SIZE, TILE_SIZE };
			SDL_RenderCopy(SDLRenderer, tileTest.GetTexture(), &texQuad, &renderQuad);
			if (map->GetCell(x, y)->ContainsActor()) {

				spr = map->GetCell(x, y)->GetActor()->GetSprite();
				texQuad = *tileTest.GetTileRect(spr->GetIndex());

				renderQuad = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
				SDL_RenderCopy(SDLRenderer, tileTest.GetTexture(), &texQuad, &renderQuad);
			}
			if (map->GetCell(x, y)->ContainsProp()) {

				spr = map->GetCell(x, y)->GetProp()->GetSprite();
				texQuad = *tileTest.GetTileRect(spr->GetIndex());

				renderQuad = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
				SDL_RenderCopy(SDLRenderer, tileTest.GetTexture(), &texQuad, &renderQuad);
			}
		}
	}

	//Update screen
	SDL_RenderPresent(SDLRenderer);
}

bool InitRenderer() {
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
		if (SDLWindow == nullptr)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			SDLRenderer = SDL_CreateRenderer(SDLWindow, -1, SDL_RENDERER_ACCELERATED);
			if (SDLRenderer == nullptr)
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
		if (!LoadMedia()) {
			success = false;
		}
	}
	return success;
}

bool LoadMedia(){
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

void CloseRenderer()
{
	//Destroy window	
	SDL_DestroyRenderer(SDLRenderer);
	SDL_DestroyWindow(SDLWindow);
	SDLWindow = nullptr;
	SDLRenderer = nullptr;
	tileTest.Free();

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}