#include <stdio.h>
#include <iostream>
#include <filesystem>
#include <SDL.h>
#include <SDL_image.h>
#include <fstream>

#include "RoomConverter.h"
#include "JSONHandler.h"
#include "ETexture.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/ostreamwrapper.h"

void RoomConverter::ConvertRooms(std::string pathName, SDL_Renderer* renderer, SDL_Window* window) {
	
	//clear out old jsons
	std::string outputPath = "room_jsons\\";
	std::string command = "del /Q ";
	std::string path = outputPath + "*.json";
	system(command.append(path).c_str());

	//loop through each room texture
	for (const auto& entry : std::filesystem::directory_iterator(pathName)) {
		std::string path = entry.path().u8string();
		std::string fileName = entry.path().stem().u8string();
		ETexture roomTexture{};
		
		rapidjson::Document doc;
		auto& allocator = doc.GetAllocator();
		doc.SetObject();
		rapidjson::Value width(rapidjson::Type::kNumberType);
		rapidjson::Value height(rapidjson::Type::kNumberType);
		rapidjson::Value walls(rapidjson::Type::kArrayType);
		rapidjson::Value floors(rapidjson::Type::kArrayType);
		rapidjson::Value doors(rapidjson::Type::kArrayType);
		rapidjson::Value openWalls(rapidjson::Type::kArrayType);

		if (!roomTexture.LoadFromFileForModifying(path, renderer, window)) {
			printf("Failed to load room texture: %s\n", path.c_str());
		}
		else {
			int w = roomTexture.GetWidth();
			int h = roomTexture.GetHeight();
			width.SetInt(w);
			height.SetInt(h);
			//Lock texture
			if (!roomTexture.LockTexture()) {
				printf("Unable to lock room texture!\n");
			}
			else
			{
				//Allocate format from window
				Uint32 format = SDL_GetWindowPixelFormat(window);
				SDL_PixelFormat* mappingFormat = SDL_AllocFormat(format);

				//Get pixel data
				Uint32* pixels = (Uint32*)roomTexture.GetPixels();
				int pixelCount = (roomTexture.GetPitch() / 4) * h;
				//Map colors
				Uint32 wallColor = SDL_MapRGB(mappingFormat, 255, 255, 255);
				Uint32 floorColor = SDL_MapRGB(mappingFormat, 0, 0, 255);
				Uint32 openWallsColor = SDL_MapRGB(mappingFormat, 255, 0, 0);
				Uint32 doorColor = SDL_MapRGB(mappingFormat, 0, 255, 0);
				
				//check key pixels
				for (int i = 0; i < pixelCount; ++i) {
					rapidjson::Value pos(rapidjson::Type::kArrayType);
					pos.PushBack(i % w, allocator);
					pos.PushBack((int)(i / w), allocator);

					if (pixels[i] == wallColor) {
						walls.PushBack(pos, allocator);
					}
					if (pixels[i] == floorColor) {
						floors.PushBack(pos, allocator);
					}
					if (pixels[i] == openWallsColor) {
						openWalls.PushBack(pos, allocator);
					}
					if (pixels[i] == doorColor) {
						doors.PushBack(pos, allocator);
					}
				}

				//create JSON
				doc.AddMember("width", width, allocator);
				doc.AddMember("height", height, allocator);
				doc.AddMember("walls", walls, allocator);
				doc.AddMember("floors", floors, allocator);
				doc.AddMember("doors", doors, allocator);
				doc.AddMember("openWalls", openWalls, allocator);

				std::ofstream output(outputPath + fileName + ".json");
				rapidjson::OStreamWrapper osw(output);
				rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
				writer.SetFormatOptions(rapidjson::kFormatSingleLineArray);
				doc.Accept(writer);

				//Unlock texture
				roomTexture.UnlockTexture();

				//Free format
				SDL_FreeFormat(mappingFormat);
			}
		}
	}
}