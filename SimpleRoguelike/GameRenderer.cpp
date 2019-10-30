#include <SDL.h>
#include <SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <iostream>

#include "GameRenderer.h"
#include "GameLoop.h"
#include "ETexture.h"
#include "Map.h"
#include "Actor.h"
#include "Sprite.h"
#include "Prop.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int TILE_SIZE = 64;

//The window we'll be rendering to
SDL_Window* SDLWindow = nullptr;

//The window renderer
SDL_Renderer* SDLRenderer = nullptr;

//Tileset texture:
ETexture tileTest;

GameLoop* game = nullptr;
Actor** playerPtr = nullptr;

bool InitRenderer();
bool LoadMedia();
void CloseRenderer();
void RenderMap(Map* map);
int tileCoordXToScreenCoordX(int x);
int tileCoordYToScreenCoordY(int y);

GameRenderer::GameRenderer() {
	InitRenderer();
}

GameRenderer::~GameRenderer() {
	CloseRenderer();
}

void GameRenderer::Render() {
	if (game->GetCurrentMap() != nullptr) {
		RenderMap(game->GetCurrentMap());
	}
}
void GameRenderer::SetGameLoop(GameLoop* loop) {
	game = loop;
	playerPtr = game->GetPlayerPtr();
}

void RenderMap(Map* map) {
	SDL_SetRenderDrawColor(SDLRenderer, 31, 14, 28, 0x00);
	SDL_RenderClear(SDLRenderer);

	int height = map->GetHeight();
	int width = map->GetWidth();
	int centerX = (*playerPtr)->GetX();
	int centerY = (*playerPtr)->GetY();

	//width + height of tiles to display:
	int tilesX = ceil(SCREEN_WIDTH / TILE_SIZE) + 1;
	int tilesY = ceil(SCREEN_HEIGHT / TILE_SIZE) + 1;

	//range of tiles to render
	int tileMapX1 = centerX - (tilesX * 0.5);
	int tileMapX2 = tileMapX1 + tilesX;
	int tileMapY1 = centerY - (tilesY * 0.5);
	int tileMapY2 = tileMapY1 + tilesY;

	//offset so tiles are drawn from top left of screen


	for (int y = tileMapY1;  y <= tileMapY2; y++) {
		for (int x = tileMapX1; x <= tileMapX2; x++) {
			if (!map->ValidPos(x, y)) {
				continue;
			}
			Sprite* spr = map->GetCell(x, y)->GetSprite();
			SDL_Rect texQuad = *tileTest.GetTileRect(spr->GetIndex());
			int renderX = tileCoordXToScreenCoordX(x);
			int renderY = tileCoordYToScreenCoordY(y);
			SDL_Rect renderQuad = { renderX, renderY, TILE_SIZE, TILE_SIZE };
			SDL_RenderCopy(SDLRenderer, tileTest.GetTexture(), &texQuad, &renderQuad);
			if (map->GetCell(x, y)->ContainsProp()) {

				spr = map->GetCell(x, y)->GetProp()->GetSprite();
				texQuad = *tileTest.GetTileRect(spr->GetIndex());

				SDL_RenderCopy(SDLRenderer, tileTest.GetTexture(), &texQuad, &renderQuad);
			}
			if (map->GetCell(x, y)->ContainsActor()) {

				spr = map->GetCell(x, y)->GetActor()->GetSprite();
				texQuad = *tileTest.GetTileRect(spr->GetIndex());

				SDL_RenderCopy(SDLRenderer, tileTest.GetTexture(), &texQuad, &renderQuad);
			}
		}
	}

	//Update screen
	SDL_RenderPresent(SDLRenderer);
}

int tileCoordXToScreenCoordX(int x) {
	int playerX = (*playerPtr)->GetX() * TILE_SIZE;
	int desiredPlayerRenderPos = (SCREEN_WIDTH * 0.5) - (TILE_SIZE * 0.5);
	int shiftAmount = desiredPlayerRenderPos - playerX;
	return (x * TILE_SIZE) + shiftAmount;
}
int tileCoordYToScreenCoordY(int y) {
	int playerY = (*playerPtr)->GetY() * TILE_SIZE;
	int desiredPlayerRenderPos = (SCREEN_HEIGHT * 0.5) - (TILE_SIZE * 0.5);
	int shiftAmount = desiredPlayerRenderPos - playerY;
	return (y * TILE_SIZE) + shiftAmount;
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