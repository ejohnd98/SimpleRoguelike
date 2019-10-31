#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>

#include "ETexture.h";


ETexture::ETexture() {
	texture = NULL;
	width = NULL;
	height = NULL;
	tileW = NULL;
	tileH = NULL;
}

//Deallocates memory
ETexture::~ETexture() {
	//Deallocate
	Free();
}

//Loads image at specified path
bool ETexture::LoadFromFile(std::string path, SDL_Renderer* renderer) {
	
	Free(); //Get rid of preexisting texture
	
	SDL_Texture* newTexture = nullptr; //The final texture
	SDL_Surface* loadedSurface = IMG_Load(path.c_str()); //Load image at specified path
	if (loadedSurface == nullptr)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface); //Create texture from surface pixels
		if (newTexture == nullptr)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			width = loadedSurface->w;
			height = loadedSurface->h;
		}
		SDL_FreeSurface(loadedSurface); //Get rid of old loaded surface
	}
	texture = newTexture;
	return texture != nullptr; //Return success
}

//Deallocates texture
void ETexture::Free() {
	//Free texture if it exists
	if (texture != nullptr)
	{
		SDL_DestroyTexture(texture);
		texture = nullptr;
		width = 0;
		height = 0;
		tileW = 0;
		tileH = 0;
	}
}

void ETexture::SetTileSetInfo(int w, int h) {
	tileW = w;
	tileH = h;
}

SDL_Rect* ETexture::GetTileRect(int tileNum) { //returns rect corresponding to tile number (top right -> bottom left)
	int columns = width / tileW;
	int rows = height / tileH;
	int x = tileNum;
	int y = 0;
	
	while (x > columns - 1) {
		y++;
		x -= columns;
	}

	SDL_Rect tileQuad = { x*tileW, y*tileH, tileW, tileH };
	return &tileQuad;
}

//Gets image dimensions
int ETexture::GetWidth() {
	return width;
}
int ETexture::GetHeight() {
	return height;
}
int ETexture::GetTileWidth() {
	return tileW;
}
int ETexture::GetTileHeight() {
	return tileH;
}
SDL_Texture* ETexture::GetTexture() {
	return texture;
}