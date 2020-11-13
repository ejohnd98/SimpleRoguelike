#include <stdio.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <SDL.h>
#include <SDL_image.h>
#include <math.h>

#include "RendererSystem.h"
#include "ETexture.h"
#include "Game.h"
#include "ECS.h"

extern std::shared_ptr<ECS> ecs;
extern std::shared_ptr<Game> game;
extern std::shared_ptr<MapSystem> mapSystem;
extern std::shared_ptr<PlayerSystem> playerSystem;
extern std::shared_ptr<LogSystem> logSystem;

const char* WINDOW_TITLE = "Roguelike";

//Render resolution:
const int RENDER_WIDTH = 1366;
const int RENDER_HEIGHT = 768;
const int PIXEL_MULT = 2; 
const int NATIVE_WIDTH = RENDER_WIDTH / PIXEL_MULT;
const int NATIVE_HEIGHT = RENDER_HEIGHT / PIXEL_MULT;

//Output resolution
const int OUTPUT_WIDTH = 1366;
const int OUTPUT_HEIGHT = 768;

RendererSystem::~RendererSystem() {
	Close();
}

void RendererSystem::Init() {
	bool success = true;

	//initialize SDL renderer
	assert(SDL_Init(SDL_INIT_VIDEO) >= 0);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

	//Create window
	SDLWindow = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, OUTPUT_WIDTH, OUTPUT_HEIGHT, SDL_WINDOW_SHOWN);
	assert(SDLWindow);

	//Create renderer for window
	SDLRenderer = SDL_CreateRenderer(SDLWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	assert(SDLRenderer != nullptr);

	//Create texture to render to
	screenRenderTexture = SDL_CreateTexture(SDLRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, RENDER_WIDTH, RENDER_HEIGHT);
	float scale1 = (float)OUTPUT_WIDTH / (float)RENDER_WIDTH;
	float scale2 = (float)OUTPUT_HEIGHT / (float)RENDER_HEIGHT;
	renderTextureScale = scale1 < scale2 ? scale1 : scale2;

	//Set renderer color
	SDL_SetRenderDrawColor(SDLRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

	//InitRenderer PNG loading
	int imgFlags = IMG_INIT_PNG;
	assert(IMG_Init(imgFlags) & imgFlags);

	assert(LoadMedia());
	assert(tilesets.find(MAIN_TILESET) != tilesets.end());
	assert(tilesets.at(MAIN_TILESET).GetTileWidth() != 0 && tilesets.at(MAIN_TILESET).GetTileHeight() != 0);
	std::cout << "Native res: " << NATIVE_WIDTH << "x" << NATIVE_HEIGHT << ", scaling x"<< renderTextureScale << "\n";
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
	SDL_SetRenderDrawColor(SDLRenderer, 0, 0, 0, 0x00);
	SDL_RenderClear(SDLRenderer);
	
	if (game->GeneratingMap()) {
		RenderMapGen(game->dunGen);
	}else{
		//render game at native res
		RenderMap(mapSystem->map);
		//render UI
		RenderUI();
	}
	RenderString({ 4, 16 }, { 603 - 8, 10 }, { 0, 2 }, std::to_string(ecs->GetEntityCount()), (Tileset)"6x8_font", PIXEL_MULT, { 1, 1 });

	//put rendertexture on screen
	SDL_SetRenderTarget(SDLRenderer, NULL);

	int renderWidth = (int)(RENDER_WIDTH * renderTextureScale);
	int renderHeight = (int)(RENDER_HEIGHT * renderTextureScale);

	SDL_Rect renderQuad = { (OUTPUT_WIDTH - renderWidth) * 0.5f, (OUTPUT_HEIGHT - renderHeight) * 0.5f, renderWidth, renderHeight };
	SDL_RenderCopy(SDLRenderer, screenRenderTexture, NULL, &renderQuad);

	SDL_RenderPresent(SDLRenderer);
}

void RendererSystem::RenderMap(std::shared_ptr<Map> map) {
	int height = map->height;
	int width = map->width;
	tileScreenSize = tilesets.at(MAIN_TILESET).GetTileWidth()* PIXEL_MULT;

	//number of tiles to render
	int horzTiles = ceil(RENDER_WIDTH / tileScreenSize) + 2;
	int vertTiles = ceil(RENDER_HEIGHT / tileScreenSize) + 2;

	Entity player = playerSystem->GetPlayerEntity();
	if (ecs->HasComponent<Active>(player)) {
		lastPlayerPos = ecs->GetComponent<Renderable>(player).position;
	}
	TileToCenter = lastPlayerPos;
	TileToCenter.y += 3;
	TileToCenter.x += 6;

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

	int numberOfAnimations = 0;
	for (auto const& entity : entities) { //iterate through renderable entities
		
		Renderable& r = ecs->GetComponent<Renderable>(entity);
		FloatPosition pos;
		pos = r.position;	
		Sprite spr = r.sprite;
		bool remove = false;
		bool visible = mapSystem->IsVisible(pos.x, pos.y);
		
		if (ecs->HasComponent<AnimSprite>(entity)) {
			AnimSprite& anim = ecs->GetComponent<AnimSprite>(entity);
			numberOfAnimations++;
			if (anim.finished || !visible) {
				//animating = false;
				ecs->RemoveComponent<AnimSprite>(entity);
				if (ecs->HasComponent<DeleteAfterAnim>(entity)) {
					remove = true;
				}
				numberOfAnimations--;
			}
			else {
				anim.AnimStep();
				spr = anim.CurrentSprite();
			}
		}
		else if (visible && ecs->HasComponent<AnimIdle>(entity)) {
			AnimIdle& anim = ecs->GetComponent<AnimIdle>(entity);
			anim.AnimStep();
			spr = anim.CurrentSprite();
		}
		
		if (ecs->HasComponent<AnimMove>(entity)) {
			AnimMove& anim = ecs->GetComponent<AnimMove>(entity);
			numberOfAnimations++;
			anim.AnimStep();
			pos = anim.CurrentPos();
			r.position = anim.CurrentPos();
			if (anim.finished || (!mapSystem->IsVisible(anim.start.x, anim.start.y) && !mapSystem->IsVisible(anim.end.x, anim.end.y))) {
				r.position = anim.end;
				ecs->RemoveComponent<AnimMove>(entity);
				if (ecs->HasComponent<DeleteAfterAnim>(entity)) {
					remove = true;
				}
				numberOfAnimations--;
			}
		}
		if (mapSystem->IsVisible(pos.x + 0.5f, pos.y + 0.5f)) {
			RenderTile(pos, spr, r.tileset, PIXEL_MULT, true, {});
		}
		if (remove) {
			ecs->DestroyEntity(entity);
		}
	}
	animating = numberOfAnimations > 0;

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
	
	FloatPosition desiredScreenPos = { RENDER_WIDTH * 0.5, RENDER_HEIGHT * 0.5 }; //set to center of screen
	desiredScreenPos = desiredScreenPos - tileScreenSize * 0.5; //offset by half tile size to correctly center tile

	FloatPosition offset = desiredScreenPos - centerTileScreenPos; //difference in where tile would be rendered and where we want to render it
	FloatPosition screenPos = (tilePos * tileScreenSize) + offset; //resultant position in pixels
	return Position{ screenPos }; // dealing with pixels now so round to int position
}

void RendererSystem::RenderUI() {
	DrawUIRect({ 0, 282 }, { 503, 102 }, (Tileset)"8x7_ui", PIXEL_MULT);
	DrawUIRect({ 502, 203 }, { 181, 151 }, (Tileset)"8x7_ui", PIXEL_MULT);
	DrawUIRect({ 502, 94 }, { 181, 110 }, (Tileset)"8x7_ui", PIXEL_MULT);
	DrawUIRect({ 502, 0 }, { 87, 95 }, (Tileset)"8x7_ui", PIXEL_MULT);
	DrawUIRect({ 588, 0 }, { 95, 95 }, (Tileset)"8x7_ui", PIXEL_MULT);
	
	DrawUIRect({ 502, 353 }, { 31, 31 }, (Tileset)"8x7_ui", PIXEL_MULT);
	DrawUIRect({ 532, 353 }, { 31, 31 }, (Tileset)"8x7_ui", PIXEL_MULT);
	DrawUIRect({ 562, 353 }, { 31, 31 }, (Tileset)"8x7_ui", PIXEL_MULT);
	DrawUIRect({ 592, 353 }, { 31, 31 }, (Tileset)"8x7_ui", PIXEL_MULT);
	DrawUIRect({ 622, 353 }, { 31, 31 }, (Tileset)"8x7_ui", PIXEL_MULT);
	DrawUIRect({ 652, 353 }, { 31, 31 }, (Tileset)"8x7_ui", PIXEL_MULT);
	
	auto&& logs = logSystem->GetLogs(10);
	int height = 369;
	for (auto&& log : logs) {
		if (height < 285) {
			break;
		}
		RenderString({ 4, height }, { 503 - 8, 10 }, { 0, 2 },
			log.text, (Tileset)"6x8_font", PIXEL_MULT, { 1, 1 });
		height -= 10;
	}

	//debug info
	Entity player = playerSystem->GetPlayerEntity();
	if (player != NULL_ENTITY) {
		Position playerPos = ecs->GetComponent<Position>(player);
		RenderString({ 4, 4 }, { 503 - 8, 10 }, { 0, 2 }, playerPos.ToString(), (Tileset)"6x8_font", PIXEL_MULT, { 1, 1 });
	}
}

void RendererSystem::DrawUIRect(Position pos, Position size, Tileset tileset, int scale) {
	int w = tilesets.at(tileset).GetTileWidth() * scale;
	int h = tilesets.at(tileset).GetTileHeight() * scale;
	Position screenPos = pos * scale;
	Position screenSize = size * scale;

	SDL_Rect renderQuad = { screenPos.x, screenPos.y, w, h };
	for (int i = 0; i < 9; i++) {
		if (i % 3 == 0) {
			renderQuad.x = screenPos.x;
			renderQuad.w = w;
		}
		if (i % 3 == 1) {
			renderQuad.x += w;
			renderQuad.w = screenSize.x - (w * 2);
		}
		if (i % 3 == 2) {
			renderQuad.x += screenSize.x - (w * 2);;
			renderQuad.w = w;
		}
		if (i == 3) {
			renderQuad.y += h;
			renderQuad.h = screenSize.y - (h * 2);
		}
		if (i == 6) {
			renderQuad.y += screenSize.y - (h * 2);
			renderQuad.h = h;
		}
		SDL_RenderCopy(SDLRenderer, tilesets.at(tileset).GetTexture(), tilesets.at(tileset).GetTileRect(i), &renderQuad);
	}

}
void RendererSystem::RenderString(Position pos, Position area, Position spacing, std::string str, Tileset font, int scale, FloatPosition fontScale) {
	int fontW = tilesets.at(font).GetTileWidth() * PIXEL_MULT * fontScale.x;
	int fontH = tilesets.at(font).GetTileHeight() * PIXEL_MULT * fontScale.y;
	Position screenPos = pos * scale;
	Position screenArea = area * scale;
	Position screenSpacing = spacing * scale;

	Position curr = screenPos;
	bool firstChar = true;
	int formatLength = 0;
	bool readingFormat = false;
	SDL_Color color = COLOR_MAP.at("white");
	for (std::string::size_type i = 0; i < str.size(); ++i) {
		int ascii = str[i];
		if (firstChar && (ascii == 0 || ascii == 32 || ascii == 255)) {
			continue;
		}
		if (ascii == '<' && !readingFormat) {
			readingFormat = true; //no, <red>yes
			formatLength = 0;
			continue;
		}
		if (readingFormat) {
			if (ascii == '>') {
				auto mapIt = COLOR_MAP.find(str.substr(i - formatLength, formatLength));
				if (mapIt != COLOR_MAP.end()) {
					color = (*mapIt).second;
				}
				else {
					color = COLOR_MAP.at("white");
				}

				SDL_SetTextureColorMod(tilesets.at(font).GetTexture(), color.r, color.g, color.b);
				readingFormat = false;
				continue;
			}
			formatLength++;
			continue;
		}
		if (str[i] == 10) { //newline char
			curr.x = screenPos.x;
			curr.y += fontH + screenSpacing.y;
			firstChar = true;
			continue;
		}
		if (str[i] == 10 || curr.x + fontW > screenPos.x + screenArea.x) { //line is full
			curr.x = screenPos.x;
			curr.y += fontH + screenSpacing.y;
			firstChar = true;
			if (ascii == 0 || ascii == 32 || ascii == 255) {
				continue;
			}
		}
		SDL_Rect texQuad = *tilesets.at(font).GetTileRect(ascii);
		SDL_Rect renderQuad = { curr.x, curr.y, fontW, fontH };
		SDL_RenderCopy(SDLRenderer, tilesets.at(font).GetTexture(), &texQuad, &renderQuad);

		curr.x += fontW + screenSpacing.x;
		firstChar = false;
	}
	SDL_SetTextureColorMod(tilesets.at(font).GetTexture(), 255, 255, 255);
}

bool RendererSystem::LoadMedia() {
	LoadTilesetsFromPath("tilesets");
	LoadTilesetsFromPath("fonts");

	RoomConverter roomConverter = RoomConverter();
	roomConverter.ConvertRooms("rooms", SDLRenderer, SDLWindow);

	return !tilesets.empty();
}

void RendererSystem::LoadTilesetsFromPath(std::string pathName) {
	for (const auto& entry : std::filesystem::directory_iterator(pathName)) {
		bool success = false;
		ETexture tex{};
		std::string path = entry.path().u8string();
		std::string fileName = entry.path().stem().u8string();
		tilesets.insert({ fileName, tex });

		tilesets.at(fileName).LoadFromFile(path, SDLRenderer);
		Position tileSize = GetTilesetSizeFromName(fileName);
		tilesets.at(fileName).SetTileSetInfo(tileSize);

		bool load = (tileSize.x != 0 && tileSize.y != 0);
		success |= load;
		std::cout << (success ? "Loaded \"" : "Failed loading ") << fileName << "\" with size of: (" << tileSize.x << ", " << tileSize.y << ")\n";
	}
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

void RendererSystem::RenderMapGen(std::shared_ptr<DungeonGenerator> dunGen) {
	std::shared_ptr<Map> map = dunGen->mapGen->map;
	int tileSize = std::min(RENDER_WIDTH / map->width, RENDER_HEIGHT / map->height);

	std::unordered_map<LayoutInfo, Sprite> spriteMap{
		{LayoutInfo::EMPTY, 15 },
		{LayoutInfo::FLOOR, 13},
		{LayoutInfo::WALL, 14 },
		{LayoutInfo::POSSIBLE_WALL, 10 },
		{LayoutInfo::DOOR, 12 },
		{LayoutInfo::POSSIBLE_DOOR, 11 },
		{LayoutInfo::ENTRANCE, 9 },
		{LayoutInfo::EXIT, 8 }
	};

	for (int y = 0; y < map->height; y++) {
		for (int x = 0; x < map->width; x++) {
			auto tile = dunGen->mapGen->mapLayout[y][x];
			SDL_Rect texQuad = *tilesets.at(MAIN_TILESET).GetTileRect(spriteMap.at(tile));
			Position renderPos = { x * tileSize, y * tileSize };

			SDL_Rect renderQuad = { renderPos.x, renderPos.y, tileSize, tileSize };
			SDL_RenderCopy(SDLRenderer, tilesets.at(MAIN_TILESET).GetTexture(), &texQuad, &renderQuad);

			int debugIndex = dunGen->mapGen->debugGraphics[y][x];
			if (debugIndex > 0) {
				texQuad = *tilesets.at(MAIN_TILESET).GetTileRect(debugIndex);
				SDL_RenderCopy(SDLRenderer, tilesets.at(MAIN_TILESET).GetTexture(), &texQuad, &renderQuad);
			}

			//RenderString(renderPos, { tileSize, tileSize}, { 0,0 },std::to_string(mapGen->bfs[y][x]), "6x8_font", 1, {0.5f,0.5f });

		}
	}
}

bool RendererSystem::AnimationPlaying() {
	return animating;
}

void RendererSystem::SetAnimationFlag() {
	animating = true;
}
