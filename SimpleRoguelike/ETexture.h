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
	bool LoadFromFileForModifying(std::string path, SDL_Renderer* renderer, SDL_Window* window);

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

	//Pixel manipulators
	bool LockTexture();
	bool UnlockTexture();
	void* GetPixels();
	int GetPitch();

private:
	//The actual hardware texture
	SDL_Texture* texture;
	void* pixels;
	int pitch;

	//Image dimensions
	int width;
	int height;

	//Tile dimensions for tilesets
	int tileW;
	int tileH;
};