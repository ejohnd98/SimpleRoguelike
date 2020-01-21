#pragma once
#include "Map.h"
class Pathfinder
{
public:
	static class Coordinate GetPath(int x1, int y1, int x2, int y2, Map* map);
	static void FillHeatMap(int sx, int sy, Map* map, int (*heatMap)[Map::MAP_WIDTH][Map::MAP_HEIGHT]);

};