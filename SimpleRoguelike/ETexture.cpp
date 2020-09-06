#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>

#include "ETexture.h"
#include "Components.h"


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

bool ETexture::LoadFromFileForModifying(std::string path, SDL_Renderer* renderer, SDL_Window* window) {

	//Get rid of preexisting texture
	Free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Convert surface to display format
		SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_GetWindowPixelFormat(window), 0);
		if (formattedSurface == NULL)
		{
			printf("Unable to convert loaded surface to display format! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Create blank streamable texture
			newTexture = SDL_CreateTexture(renderer, SDL_GetWindowPixelFormat(window), SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h);
			if (newTexture == NULL)
			{
				printf("Unable to create blank texture! SDL Error: %s\n", SDL_GetError());
			}
			else
			{
				//Lock texture for manipulation
				SDL_LockTexture(newTexture, NULL, &pixels, &pitch);

				//Copy loaded/formatted surface pixels
				memcpy(pixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h);

				//Unlock texture to update
				SDL_UnlockTexture(newTexture);
				pixels = NULL;

				//Get image dimensions
				width = formattedSurface->w;
				height = formattedSurface->h;
			}

			//Get rid of old formatted surface
			SDL_FreeSurface(formattedSurface);
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	texture = newTexture;
	return texture != NULL;
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

void ETexture::SetTileSetInfo(Position tileSize) {
	tileW = tileSize.x;
	tileH = tileSize.y;
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

bool ETexture::LockTexture()
{
	bool success = true;

	//Texture is already locked
	if (pixels != NULL)
	{
		printf("Texture is already locked!\n");
		success = false;
	}
	//Lock texture
	else
	{
		if (SDL_LockTexture(texture, NULL, &pixels, &pitch) != 0)
		{
			printf("Unable to lock texture! %s\n", SDL_GetError());
			success = false;
		}
	}

	return success;
}

bool ETexture::UnlockTexture()
{
	bool success = true;

	//Texture is not locked
	if (pixels == NULL)
	{
		printf("Texture is not locked!\n");
		success = false;
	}
	//Unlock texture
	else
	{
		SDL_UnlockTexture(texture);
		pixels = NULL;
		pitch = 0;
	}

	return success;
}

void* ETexture::GetPixels()
{
	return pixels;
}

int ETexture::GetPitch()
{
	return pitch;
}