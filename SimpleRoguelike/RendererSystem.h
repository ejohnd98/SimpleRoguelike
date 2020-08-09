#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <SDL.h>
#include <SDL_image.h>

#include "System.h"
#include "ETexture.h"

class RendererSystem : public System
{
public:
	~RendererSystem();
	void Init();
	void Close();
	void Render();

	bool AnimationPlaying();

private:
	bool animating = false;
	void RenderMap(std::shared_ptr<Map> map);
	void RenderTile(FloatPosition pos, Sprite spr, Tileset tileset, int scale);
	Position TilePosToScreenPos(FloatPosition tilePos);
	bool LoadMedia();
	Position GetTilesetSizeFromName(std::string name);

	SDL_Window* SDLWindow = nullptr; //The window that will be rendered to
	SDL_Renderer* SDLRenderer = nullptr; //The window renderer

	std::unordered_map<Tileset, ETexture> tilesets;
	ETexture fontTex;

	FloatPosition lastPlayerPos;
	FloatPosition TileToCenter; //position in-game that will be rendered in center
	int centerTileX;
	int centerTileY;
	int tileScreenSize;
};