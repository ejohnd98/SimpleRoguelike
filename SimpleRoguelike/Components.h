#pragma once
#include <string>
const int MAX_MAP_SIZE = 100;

struct Map {
	int mapWidth;
	int mapHeight;

	std::shared_ptr<bool[MAX_MAP_SIZE][MAX_MAP_SIZE]> cells;

	std::shared_ptr<bool[MAX_MAP_SIZE][MAX_MAP_SIZE]> visited;
	std::shared_ptr<bool[MAX_MAP_SIZE][MAX_MAP_SIZE]> visible;
};

struct Position {
	int x = 0;
	int y = 0;
};

struct Actor {
	int actionDebt = 0;
};

struct Renderable {
	int sprite = 0;
	float x = 0;
	float y = 0;
};

struct Info {
	std::string name = "Untitled";
	std::string info = "No description.";
};

struct PlayerControlled {

};

