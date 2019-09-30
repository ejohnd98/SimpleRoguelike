#include <stdio.h>
#include "Map.h"
#include "Cell.h"

const int MAP_WIDTH = 10;
const int MAP_HEIGHT = 10;

Cell* cellMap[MAP_WIDTH][MAP_HEIGHT];

void InitMap();
void FreeMap();

Map::Map() {
	InitMap();
}

Map::~Map() {
	FreeMap();
}

void InitMap() {

	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			cellMap[x][y] = new Cell();
			bool setWall = false;
			if (x == 0 || y == 0 || x == MAP_WIDTH-1 || y == MAP_HEIGHT-1) {
				setWall = true;
			}
			cellMap[x][y]->SetupCell(setWall);
		}
	}

}

void FreeMap() {

	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			delete cellMap[x][y];
		}
	}

}

Cell* Map::GetCell(int x, int y) {
	return cellMap[x][y];
}