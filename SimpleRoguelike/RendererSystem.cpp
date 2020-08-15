#include <stdio.h>
#include <string>
#include <iostream>
#include <filesystem>
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

//Render resolution:
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 1600;
const int PIXEL_MULT = 8; 

//Output resolution
const int EXTERNAL_SCREEN_WIDTH = 800;
const int EXTERNAL_SCREEN_HEIGHT = 800;

const Tileset MAIN_TILESET = "16x16_tileset";
const int TILESET_WIDTH = 16;
const int TILESET_HEIGHT = 16;

RendererSystem::~RendererSystem() {
	Close();
}

void RendererSystem::Init() {
	bool success = true;

	//initialize SDL renderer
	assert(SDL_Init(SDL_INIT_VIDEO) >= 0);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

	//Create window
	SDLWindow = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, EXTERNAL_SCREEN_WIDTH, EXTERNAL_SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	assert(SDLWindow);

	//Create renderer for window
	SDLRenderer = SDL_CreateRenderer(SDLWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	assert(SDLRenderer != nullptr);

	//Create texture to render to
	screenRenderTexture = SDL_CreateTexture(SDLRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
	float scale1 = (float)EXTERNAL_SCREEN_WIDTH / (float)SCREEN_WIDTH;
	float scale2 = (float)EXTERNAL_SCREEN_HEIGHT / (float)SCREEN_HEIGHT;
	renderTextureScale = scale1 < scale2 ? scale1 : scale2;

	//Set renderer color
	SDL_SetRenderDrawColor(SDLRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

	//InitRenderer PNG loading
	int imgFlags = IMG_INIT_PNG;
	assert(IMG_Init(imgFlags) & imgFlags);

	assert(LoadMedia());
	assert(tilesets.find(MAIN_TILESET) != tilesets.end());
	assert(tilesets.at(MAIN_TILESET).GetTileWidth() != 0 && tilesets.at(MAIN_TILESET).GetTileHeight() != 0);
}

void RendererSystem::Close()
{
	//Destroy window	
	SDL_DestroyRenderer(SDLRenderer);
	SDL_DestroyWindow(SDLWindow);
	SDLWindow = nullptr;
	SDLRenderer = nullptr;

	for (auto& it : tilesets) {
		it.second.Free();
	}

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

void RendererSystem::Render() {
	SDL_SetRenderTarget(SDLRenderer, screenRenderTexture);
	SDL_SetRenderDrawColor(SDLRenderer, 31, 14, 28, 0x00);
	SDL_RenderClear(SDLRenderer);

	RenderMap(mapSystem->map);

	SDL_SetRenderTarget(SDLRenderer, NULL);

	int renderWidth = (int)(SCREEN_WIDTH * renderTextureScale);
	int renderHeight = (int)(SCREEN_HEIGHT * renderTextureScale);

	SDL_Rect renderQuad = { (EXTERNAL_SCREEN_WIDTH - renderWidth) * 0.5f, (EXTERNAL_SCREEN_HEIGHT - renderHeight) * 0.5f, renderWidth, renderHeight };
	SDL_RenderCopy(SDLRenderer, screenRenderTexture, NULL, &renderQuad);

	SDL_RenderPresent(SDLRenderer);
}

void RendererSystem::RenderMap(std::shared_ptr<Map> map) {
	int height = map->height;
	int width = map->width;
	tileScreenSize = tilesets.at(MAIN_TILESET).GetTileWidth()* PIXEL_MULT;

	//number of tiles to render
	int horzTiles = ceil(SCREEN_WIDTH / tileScreenSize) + 2;
	int vertTiles = ceil(SCREEN_HEIGHT / tileScreenSize) + 2;

	Entity player = playerSystem->GetPlayerEntity();
	if (ecs->HasComponent<Active>(player)) {
		lastPlayerPos = ecs->GetComponent<Renderable>(player).position;
	}
	TileToCenter = lastPlayerPos;

	cameraPos = Animation::QuadEaseOut(cameraPos, TileToCenter, 0.05f);


	//range of tiles to render
	int renderX1 = (int)(cameraPos.x - ceil(horzTiles * 0.5)) - 1;
	int renderX2 = (int)(cameraPos.x + ceil(horzTiles * 0.5)) + 1;
	int renderY1 = (int)(cameraPos.y - ceil(vertTiles * 0.5)) - 1;
	int renderY2 = (int)(cameraPos.y + ceil(vertTiles * 0.5)) + 1;

	for (int y = renderY1; y <= renderY2; y++) {
		for (int x = renderX1; x <= renderX2; x++) {
			if (!mapSystem->ValidPosition({ x, y }) || (!mapSystem->IsVisible(x, y) && !mapSystem->IsKnown(x, y))) { //if position is invalid or not visible/known
				//RenderTile({ (float)x, (float)y }, 5, MAIN_TILESET, PIXEL_MULT, true, {}); //render '?'
				continue;
			}
			Sprite spr = map->floorSprite;
			if (map->GetCell(x,y)) { //if wall
				spr = map->wallSprite;
			}
			RenderTile({ (float)x, (float)y }, spr, MAIN_TILESET, PIXEL_MULT, false, {});
			if (!mapSystem->IsVisible(x, y) && mapSystem->IsKnown(x, y)) {
				RenderTile({ (float)x, (float)y }, 6, MAIN_TILESET, PIXEL_MULT, false, {}); //render fog if position not visible but known
			}
		}
	}
	for (int y = renderY1; y <= renderY2; y++) {
		for (int x = renderX1; x <= renderX2; x++) {
			if (!mapSystem->ValidPosition({ x, y }) || (!mapSystem->IsVisible(x, y) && !mapSystem->IsKnown(x, y))) { //if position is invalid or not visible/known
				RenderTile({ (float)x, (float)y }, 0, "32x32_fogBlack", PIXEL_MULT, true, {}); //render '?'
			}
		}
	}

	animating = false;

	for (auto const& entity : entities) { //iterate through renderable entities
		
		Renderable& r = ecs->GetComponent<Renderable>(entity);
		FloatPosition pos;
		pos = r.position;	
		Sprite spr = r.sprite;
		bool remove = false;
		
		if (ecs->HasComponent<AnimSprite>(entity)) {
			AnimSprite& anim = ecs->GetComponent<AnimSprite>(entity);
			animating = true;
			if (anim.finished) {
				//animating = false;
				ecs->RemoveComponent<AnimSprite>(entity);
				if (ecs->HasComponent<DeleteAfterAnim>(entity)) {
					remove = true;
				}
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
				if (ecs->HasComponent<DeleteAfterAnim>(entity)) {
					remove = true;
				}
			}
		}
		if (mapSystem->IsVisible(pos.x, pos.y)) {
			RenderTile(pos, spr, r.tileset, PIXEL_MULT, true, {});
		}
		if (remove) {
			ecs->DestroyEntity(entity);
		}
	}

}

void RendererSystem::RenderTile(FloatPosition pos, Sprite spr, Tileset tileset, int scale, bool center, Position offset) {
	int w = tilesets.at(tileset).GetTileWidth();
	int h = tilesets.at(tileset).GetTileHeight();
	int wDiff = w - TILESET_WIDTH;
	int hDiff = h - TILESET_HEIGHT;

	SDL_Rect texQuad = *tilesets.at(tileset).GetTileRect(spr);
	Position renderPos = TilePosToScreenPos(pos) + (offset * scale);
	if (center) {
		renderPos = renderPos - Position{ (int)(wDiff * 0.5f * scale), (int)(hDiff * 0.5f * scale) };
	}

	SDL_Rect renderQuad = { renderPos.x, renderPos.y, w * scale, h * scale };
	SDL_RenderCopy(SDLRenderer, tilesets.at(tileset).GetTexture(), &texQuad, &renderQuad);
}

Position RendererSystem::TilePosToScreenPos(FloatPosition tilePos) { //converts from tile position to screen position (pixels) based on centered tile (usually the player)
	
	FloatPosition tileScreenSize = { TILESET_WIDTH, TILESET_HEIGHT }; //size of a tile in pixels
	tileScreenSize = tileScreenSize * PIXEL_MULT; //adjust for scale factor
	FloatPosition centerTileScreenPos = cameraPos * tileScreenSize; //center if rendered without offset
	
	FloatPosition desiredScreenPos = {SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.5 }; //set to center of screen
	desiredScreenPos = desiredScreenPos - tileScreenSize * 0.5; //offset by half tile size to correctly center tile

	FloatPosition offset = desiredScreenPos - centerTileScreenPos; //difference in where tile would be rendered and where we want to render it
	FloatPosition screenPos = (tilePos * tileScreenSize) + offset; //resultant position in pixels
	return Position{ screenPos }; // dealing with pixels now so round to int position
}

bool RendererSystem::LoadMedia() {
	//Loading success flag
	bool success = true;

	for (const auto& entry : std::filesystem::directory_iterator("tilesets/")) {
		ETexture tex{};
		std::string path = entry.path().u8string();
		std::string fileName = entry.path().stem().u8string();
		tilesets.insert({ fileName, tex });

		tilesets.at(fileName).LoadFromFile(path, SDLRenderer);
		Position tileSize = GetTilesetSizeFromName(fileName);
		tilesets.at(fileName).SetTileSetInfo(tileSize);
		
		bool load = (tileSize.x != 0 && tileSize.y != 0);
		success |= load;

		std::cout << "Loaded \"" << fileName << "\" with size of: " << tileSize.x << ", " << tileSize.y << "\n";

	}

	if (fontTex.LoadFromFile("fonts/font.png", SDLRenderer)) {
		fontTex.SetTileSetInfo({ 8, 12 });
	}else {
		success = false;
	}
	return success;
}

Position RendererSystem::GetTilesetSizeFromName(std::string name) {
	int width = 0;
	int height = 0;
	int i = 0;
	int xPos = 0;
	for (auto it = name.cbegin(); it != name.cend(); ++it) {
		if (*it == 'x' && width == 0) {
			width = std::stoi(name.substr(0, i));
			xPos = i;
		} else if (!isdigit(*it) && width != 0) {
			height = std::stoi(name.substr(xPos + 1, i - xPos - 1));
			break;
		}

		i++;
	}
	assert(width != 0 && height != 0);

	return Position{ width, height };
}

bool RendererSystem::AnimationPlaying() {
	return animating;
}