#include "Cell.h"
#pragma once
class Map
{
public:
	Map();
	~Map();
	Cell* GetCell(int x, int y);\
	bool IsOccupied(int x, int y)
};

