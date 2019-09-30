#pragma once
class ETexture
{
public:
	//Initializes variables
	ETexture();

	//Deallocates memory
	~ETexture();

	//Loads image at specified path
	bool LoadFromFile(std::string path, SDL_Renderer* renderer);

	void SetTileSetInfo(int tileW, int tileH);

	SDL_Rect* GetTileRect(int tileNum);

	//Deallocates texture
	void Free();

	//Gets image dimensions
	int GetWidth();
	int GetHeight();
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