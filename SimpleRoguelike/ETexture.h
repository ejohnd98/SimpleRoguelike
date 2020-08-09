#pragma once
#include<string>
#include <SDL.h>
#include <SDL_image.h>
class ETexture
{
public:
	//Initializes variables
	ETexture();

	//Deallocates memory
	~ETexture();

	//Loads image at specified path
	bool LoadFromFile(std::string path, SDL_Renderer* renderer);

	void SetTileSetInfo(struct Position tileSize); //sets tileset dimensions

	SDL_Rect* GetTileRect(int tileNum); //returns a rect corresponding to tile of given index

	//Deallocates texture
	void Free();

	//Gets image dimensions
	int GetWidth();
	int GetHeight();
	int GetTileWidth();
	int GetTileHeight();
	SDL_Texture* GetTexture();

private:
	//The actual hardware texture
	SDL_Texture* texture;

	//Image dimensions
	int width;
	int height;

	//Tile dimensions for tilesets
	int tileW;
	int tileH;
};