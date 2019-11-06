#include <SDL.h>
#include <SDL_image.h>
#include <math.h>
#include <string>
#include <stdio.h>
#include <iostream>

#include "GameRenderer.h"
#include "GameLoop.h"
#include "ETexture.h"
#include "Map.h"
#include "Actor.h"
#include "Sprite.h"
#include "Prop.h"

#include "GameLog.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int PIXEL_MULT = 2;


//The window we'll be rendering to
SDL_Window* SDLWindow = nullptr;

//The window renderer
SDL_Renderer* SDLRenderer = nullptr;

//Tileset texture:
ETexture tileset;
ETexture fontTex;

GameLoop* game = nullptr;
Actor** playerPtr = nullptr;

//GameLog* GameLog::instancePtr = 0;

bool InitRenderer();
bool LoadMedia();
void CloseRenderer();
void RenderMap(Map* map);
void RenderTile(int x, int y, int tileIndex, int tileScreenSize);
int TileCoordXToScreenCoordX(int x);
int TileCoordYToScreenCoordY(int y);
void RenderString(std::string str, int x1, int x2, int y1, int y2);

GameRenderer::GameRenderer() {
	InitRenderer();
}

GameRenderer::~GameRenderer() {
	CloseRenderer();
}

void GameRenderer::Render() {
	SDL_SetRenderDrawColor(SDLRenderer, 31, 14, 28, 0x00);
	SDL_RenderClear(SDLRenderer);
	if (game->GetCurrentMap() != nullptr) {
		RenderMap(game->GetCurrentMap());
		
		RenderString(GameLog::instance()->get_value(), 0, 640, 336, 480);
		
	}
	SDL_RenderPresent(SDLRenderer);
}
void GameRenderer::SetGameLoop(GameLoop* loop) {
	game = loop;
	playerPtr = game->GetPlayerPtr();
}

void RenderMap(Map* map) {
	
	int height = map->GetHeight();
	int width = map->GetWidth();
	int centerX = (*playerPtr)->GetX();
	int centerY = (*playerPtr)->GetY();
	int tileScreenSize = tileset.GetTileWidth() * PIXEL_MULT;

	//width + height of tiles to display:
	int tilesX = ceil(SCREEN_WIDTH / tileScreenSize) + 1;
	int tilesY = ceil(SCREEN_HEIGHT / tileScreenSize) + 1;

	//range of tiles to render
	int tileMapX1 = centerX - (tilesX * 0.5);
	int tileMapX2 = tileMapX1 + tilesX;
	int tileMapY1 = centerY - (tilesY * 0.5);
	int tileMapY2 = tileMapY1 + tilesY;

	//offset so tiles are drawn from top left of screen


	for (int y = tileMapY1;  y <= tileMapY2; y++) {
		for (int x = tileMapX1; x <= tileMapX2; x++) {
			if (!map->ValidPos(x, y)) {
				RenderTile(x, y, 2, tileScreenSize);
				continue;
			}
			Sprite* spr = map->GetCell(x, y)->GetSprite();
			RenderTile(x, y, spr->GetIndex(), tileScreenSize);
			if (map->GetCell(x, y)->ContainsProp()) {
				spr = map->GetCell(x, y)->GetProp()->GetSprite();
				RenderTile(x, y, spr->GetIndex(), tileScreenSize);
			}
			if (map->GetCell(x, y)->ContainsActor()) {
				spr = map->GetCell(x, y)->GetActor()->GetSprite();
				RenderTile(x, y, spr->GetIndex(), tileScreenSize);
			}
		}
	}
}

int TileCoordXToScreenCoordX(int x) {
	int tileScreenSize = tileset.GetTileWidth() * PIXEL_MULT;
	int playerX = (*playerPtr)->GetX() * tileScreenSize;
	int desiredPlayerRenderPos = (SCREEN_WIDTH * 0.5) - (tileScreenSize * 0.5);
	int shiftAmount = desiredPlayerRenderPos - playerX;
	return (x * tileScreenSize) + shiftAmount;
}
int TileCoordYToScreenCoordY(int y) {
	int tileScreenSize = tileset.GetTileHeight() * PIXEL_MULT;
	int playerY = (*playerPtr)->GetY() * tileScreenSize;
	int desiredPlayerRenderPos = (SCREEN_HEIGHT * 0.5) - (tileScreenSize * 0.5);
	int shiftAmount = desiredPlayerRenderPos - playerY;
	return (y * tileScreenSize) + shiftAmount;
}

void RenderTile(int x, int y, int tileIndex, int tileScreenSize){
	SDL_Rect texQuad = *tileset.GetTileRect(tileIndex);
	int renderX = TileCoordXToScreenCoordX(x);
	int renderY = TileCoordYToScreenCoordY(y);
	SDL_Rect renderQuad = { renderX, renderY, tileScreenSize, tileScreenSize };
	SDL_RenderCopy(SDLRenderer, tileset.GetTexture(), &texQuad, &renderQuad);
}

void RenderString(std::string str, int x1, int x2, int y1, int y2) {
	int fontW = fontTex.GetTileWidth() * PIXEL_MULT;
	int fontH = fontTex.GetTileHeight() * PIXEL_MULT;
	int currX = x1;
	int currY = y1;
	for (std::string::size_type i = 0; i < str.size(); ++i) {
		int ascii = str[i];
		if (str[i] == 10) { //newline char
			currX = x1;
			currY += fontH;
			continue;
		}
		if (currX + fontW > x2) { //line is full
			currX = x1;
			currY += fontH;
			if (ascii == 0 || ascii == 32 || ascii == 255) {
				continue;
			}
		}
		//std::cout << "ascii of " << str << " is " << ascii << "\n";
		SDL_Rect texQuad = *fontTex.GetTileRect(ascii);
		SDL_Rect renderQuad = { currX, currY, fontW, fontH };
		SDL_RenderCopy(SDLRenderer, fontTex.GetTexture(), &texQuad, &renderQuad);
		currX += fontW;
	}
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
		SDLWindow = SDL_CreateWindow("Simple Roguelike :^)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

	if (tileset.LoadFromFile("tileset.png", SDLRenderer)) {
		tileset.SetTileSetInfo(16, 16);
	}
	if (fontTex.LoadFromFile("font.png", SDLRenderer)) {
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