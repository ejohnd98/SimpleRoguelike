#include "Cell.h"
#include <list>
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

	bool MoveActor(class Actor* actor, int x, int y);
	bool PlaceActor(class Actor* actor,int x, int y);
	bool RemoveActor(class Actor* actor);
	bool ValidPos(int x, int y);

	std::list<Actor*> actorList = {};
};

