#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <SDL.h>
#include <SDL_image.h>

#include "System.h"
#include "ETexture.h"
#include "RoomConverter.h"
#include "MapGenerator.h"
#include "DungeonGenerator.h"

class RendererSystem : public System
{
public:
	~RendererSystem();
	void Init();
	void Close();
	void Render();

	bool AnimationPlaying();
	void SetAnimationFlag();

private:
	bool animating = false;
	void RenderMap(std::shared_ptr<Map> map);
	void RenderTile(FloatPosition pos, Sprite spr, Tileset tileset, int scale, bool center, Position offset);
	Position TilePosToScreenPos(FloatPosition tilePos);
	void RenderUI();
	void DrawUIRect(Position pos, Position size, Tileset tileset, int scale);
	void RenderString(Position pos, Position area, Position spacing, std::string str, Tileset font, int scale, FloatPosition fontScale);
	bool LoadMedia();
	void LoadTilesetsFromPath(std::string pathName);
	Position GetTilesetSizeFromName(std::string name);

	//debug
	void RenderMapGen(std::shared_ptr<DungeonGenerator> dunGen);

	SDL_Window* SDLWindow = nullptr; //The window that will be rendered to
	SDL_Renderer* SDLRenderer = nullptr; //The window renderer

	std::unordered_map<Tileset, ETexture> tilesets;
	SDL_Texture* screenRenderTexture;
	float renderTextureScale;

	FloatPosition lastPlayerPos;
	FloatPosition TileToCenter; //position in-game that will be rendered in center
	FloatPosition cameraPos;
	int centerTileX;
	int centerTileY;
	int tileScreenSize;
};