#include "Cell.h"
#pragma once
class Map
{
public:
	Map();
	~Map();
	Cell* GetCell(int x, int y); 
	bool IsOccupied(int x, int y);
	int GetHeight();
	int GetWidth();

	bool PlaceActor(class Actor* actor,int x, int y);
	bool RemoveActor(class Actor* actor);
};

