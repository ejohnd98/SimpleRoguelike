#include "MapGenerator.h"
#include "Map.h"
#include "RandomNumber.h"

Map* MapGenerator::GenerateMap(int w, int h) {
	//generation properties:
	float wallChance = 0.45;

	Map* map = new Map(w, h);
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			bool setWall = RandomNumber::GetRandomBool(wallChance);
			map->GetCell(x, y)->SetupCell(setWall);
		}
	}
	for (int i = 0; i < 2; i++) {
		bool newMap[Map::MAP_WIDTH][Map::MAP_HEIGHT];
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				int adjacentWalls = 0;
				if (map->IsWall(x + 1, y)) {
					adjacentWalls++;
				}
				if (map->IsWall(x - 1, y)) {
					adjacentWalls++;
				}
				if (map->IsWall(x, y + 1)) {
					adjacentWalls++;
				}
				if (map->IsWall(x, y - 1)) {
					adjacentWalls++;
				}

				if (map->IsWall(x, y)) {
					if (adjacentWalls < 2 || adjacentWalls > 3) {
						newMap[x][y] = false;
					}
					else if (adjacentWalls == 2 || adjacentWalls == 3) {
						newMap[x][y] = true;
					}
				}
				else if (adjacentWalls == 3) {
					newMap[x][y] = true;
				}
				else {
					newMap[x][y] = false;
				}
			}
		}
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				map->GetCell(x, y)->SetupCell(newMap[x][y]);
			}
		}
	}

	map->SetAllKnown(true); //for debugging map gen
	return map;
}