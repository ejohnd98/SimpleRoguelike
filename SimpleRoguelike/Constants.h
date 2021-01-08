#pragma once
#include <iostream>
#include <bitset>
#include <unordered_map>
#include <SDL.h>

using Entity = std::uint16_t;
using Sprite = std::uint16_t;
using Tileset = std::string;
using ComponentType = std::uint8_t;

const Entity MAX_ENTITIES = 10000;
const Entity NULL_ENTITY = 0;
const ComponentType MAX_COMPONENTS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;

const int MAX_MAP_SIZE = 100;
const int MAX_PREFAB_SIZE = 40;

const Tileset MAIN_TILESET = "16x16_tileset";
const int TILESET_WIDTH = 16;
const int TILESET_HEIGHT = 16;

const int MAX_STORED_LOGS = 50;

const bool DEBUG_MAP_GEN = false;

const std::unordered_map<std::string, SDL_Color> COLOR_MAP{
		{"red", SDL_Color{255,75,15,255}},
		{"orange", SDL_Color{255,120,100,255}},
		{"yellow", SDL_Color{255,230,70,255}},
		{"green", SDL_Color{78,255,31,255}},
		{"blue", SDL_Color{0,140,255,255}},
		{"purple", SDL_Color{178,104,255,255}},
		{"white", SDL_Color{255,255,255,255}},
		{"grey", SDL_Color{125,125,125,255}},
		{"black", SDL_Color{0,0,0,255}}
};