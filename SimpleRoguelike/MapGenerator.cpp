#include "MapGenerator.h"
#include "Map.h"

Map* MapGenerator::GenerateMap() {
	Map* map = new Map();
	int width = map->GetWidth();
	int height = map->GetHeight();
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			map->GetCell(x, y)->SetupCell(false);
		}
	}
	//map->SetAllKnown(true); //for debugging map gen
	//map->SetAllVisible(true);
	return map;
}