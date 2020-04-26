#include <stdio.h>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <math.h>

#include "RendererSystem.h"
#include "ETexture.h"
#include "ECS.h"

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr<MapSystem> mapSystem;

const char* WINDOW_TITLE = "Roguelike Rework";
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PIXEL_MULT = 2;

int centerTileX;
int centerTileY;
int tileScreenSize;

SDL_Window* SDLWindow = nullptr; //The window that will be rendered to
SDL_Renderer* SDLRenderer = nullptr; //The window renderer
ETexture tileset;
ETexture fontTex;

RendererSystem::~RendererSystem() {
	Close();
}

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

	RenderMap(mapSystem->map);

	SDL_RenderPresent(SDLRenderer);
}

void RendererSystem::RenderMap(std::shared_ptr<Map> map) {
	int height = map->height;
	int width = map->width;
	tileScreenSize = tileset.GetTileWidth()* PIXEL_MULT;

	//number of tiles to render
	int horzTiles = ceil(SCREEN_WIDTH / tileScreenSize);
	int vertTiles = ceil(SCREEN_HEIGHT / tileScreenSize);

	//center
	centerTileX = 2;
	centerTileY = 2;

	//range of tiles to render
	int renderX1 = centerTileX - ceil(horzTiles * 0.5);
	int renderX2 = centerTileX + ceil(horzTiles * 0.5);
	int renderY1 = centerTileY - ceil(vertTiles * 0.5);
	int renderY2 = centerTileY + ceil(vertTiles * 0.5);

	for (int y = renderY1; y <= renderY2; y++) {
		for (int x = renderX1; x <= renderX2; x++) {
			if (!mapSystem->ValidPosition({ x, y })) { //if position is invalid
				RenderTile(x, y, 5, tileScreenSize); //render '?'
				continue;
			}
			Sprite spr = map->floorSprite;
			if (map->cells[x][y]) { //if wall
				spr = map->wallSprite;
			}
			RenderTile(x, y, spr, tileScreenSize);
		}
	}

	for (auto const& entity : entities) { //iterate through renderable entities
		
		Renderable r = ecs->GetComponent<Renderable>(entity);
		Position pos = ecs->GetComponent<Position>(entity);
		RenderTile(pos.x, pos.y, r.sprite, tileScreenSize);
	}

}

//should change the following to use Position and condense into 1 function
int TileCoordXToScreenCoordX(int x) {
	int tileScreenSize = tileset.GetTileWidth() * PIXEL_MULT;
	int playerX = centerTileX * tileScreenSize;
	int desiredPlayerRenderPos = (SCREEN_WIDTH * 0.5) - (tileScreenSize * 0.5);
	int shiftAmount = desiredPlayerRenderPos - playerX;
	return (x * tileScreenSize) + shiftAmount;
}
int TileCoordYToScreenCoordY(int y) {
	int tileScreenSize = tileset.GetTileHeight() * PIXEL_MULT;
	int playerY = centerTileY * tileScreenSize;
	int desiredPlayerRenderPos = (SCREEN_HEIGHT * 0.5) - (tileScreenSize * 0.5);
	int shiftAmount = desiredPlayerRenderPos - playerY;
	return (y * tileScreenSize) + shiftAmount;
}

void RendererSystem::RenderTile(int x, int y, Sprite spr, int tileScreenSize) {
	SDL_Rect texQuad = *tileset.GetTileRect(spr);
	int renderX = TileCoordXToScreenCoordX(x);
	int renderY = TileCoordYToScreenCoordY(y);
	SDL_Rect renderQuad = { renderX, renderY, tileScreenSize, tileScreenSize };
	SDL_RenderCopy(SDLRenderer, tileset.GetTexture(), &texQuad, &renderQuad);
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