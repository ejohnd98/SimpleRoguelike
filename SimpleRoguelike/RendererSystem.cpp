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
extern std::shared_ptr<PlayerSystem> playerSystem;

const char* WINDOW_TITLE = "Roguelike Rework";
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PIXEL_MULT = 2;

int centerTileX;
int centerTileY;
FloatPosition TileToCenter; //position in-game that will be rendered in center
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
	int horzTiles = ceil(SCREEN_WIDTH / tileScreenSize) + 2;
	int vertTiles = ceil(SCREEN_HEIGHT / tileScreenSize) + 2;

	Entity player = playerSystem->GetPlayerEntity();
	Renderable& r = ecs->GetComponent<Renderable>(player);
	TileToCenter = r.position;


	//range of tiles to render
	int renderX1 = (int)(TileToCenter.x - ceil(horzTiles * 0.5));
	int renderX2 = (int)(TileToCenter.x + ceil(horzTiles * 0.5));
	int renderY1 = (int)(TileToCenter.y - ceil(vertTiles * 0.5));
	int renderY2 = (int)(TileToCenter.y + ceil(vertTiles * 0.5));

	for (int y = renderY1; y <= renderY2; y++) {
		for (int x = renderX1; x <= renderX2; x++) {
			if (!mapSystem->ValidPosition({ x, y })) { //if position is invalid
				RenderTile({ (float)x, (float)y }, 5, tileScreenSize); //render '?'
				continue;
			}
			Sprite spr = map->floorSprite;
			if (map->GetCell(x,y)) { //if wall
				spr = map->wallSprite;
			}
			RenderTile({ (float)x, (float)y }, spr, tileScreenSize);
		}
	}

	animating = false;

	for (auto const& entity : entities) { //iterate through renderable entities
		
		Renderable& r = ecs->GetComponent<Renderable>(entity);
		FloatPosition pos;
		pos = r.position;	
		Sprite spr = r.sprite;
		
		if (ecs->HasComponent<AnimSprite>(entity)) {
			AnimSprite& anim = ecs->GetComponent<AnimSprite>(entity);
			animating = true;
			if (anim.finished) {
				animating = false;
				ecs->RemoveComponent<AnimSprite>(entity);
			}
			else {
				anim.AnimStep();
				spr = anim.CurrentSprite();
			}
		}
		else if (ecs->HasComponent<AnimIdle>(entity)) {
			AnimIdle& anim = ecs->GetComponent<AnimIdle>(entity);
			anim.AnimStep();
			spr = anim.CurrentSprite();
		}
		
		if (ecs->HasComponent<AnimMove>(entity)) {
			AnimMove& anim = ecs->GetComponent<AnimMove>(entity);
			animating = true;
			anim.AnimStep();
			pos = anim.CurrentPos();
			r.position = anim.CurrentPos();
			if (anim.finished) {
				ecs->RemoveComponent<AnimMove>(entity);
				animating = false;
			}
		}
		RenderTile(pos, spr, tileScreenSize);
	}

}

Position TilePosToScreenPos(FloatPosition tilePos) { //converts from tile position to screen position (pixels) based on centered tile (usually the player)
	
	FloatPosition tileScreenSize = { tileset.GetTileWidth(), tileset.GetTileHeight() }; //size of a tile in pixels
	tileScreenSize = tileScreenSize * PIXEL_MULT; //adjust for scale factor
	FloatPosition centerTileScreenPos = TileToCenter * tileScreenSize; //center if rendered without offset
	
	FloatPosition desiredScreenPos = {SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.5 }; //set to center of screen
	desiredScreenPos = desiredScreenPos - tileScreenSize * 0.5; //offset by half tile size to correctly center tile

	FloatPosition offset = desiredScreenPos - centerTileScreenPos; //difference in where tile would be rendered and where we want to render it
	FloatPosition screenPos = (tilePos * tileScreenSize) + offset; //resultant position in pixels
	return Position{ screenPos }; // dealing with pixels now so round to int position
}

void RendererSystem::RenderTile(FloatPosition pos, Sprite spr, int tileScreenSize) {
	
	SDL_Rect texQuad = *tileset.GetTileRect(spr);
	Position renderPos = TilePosToScreenPos(pos);
	
	SDL_Rect renderQuad = { renderPos.x, renderPos.y, tileScreenSize, tileScreenSize };
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

bool RendererSystem::AnimationPlaying() {
	return animating;
}