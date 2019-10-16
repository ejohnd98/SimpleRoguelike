#include "Cell.h"
#include <list>
#pragma once
class Map
{
public:
	const int MAP_WIDTH = 20;
	const int MAP_HEIGHT = 15;
	std::list<Actor*> actorList = {};

	Map();
	~Map();
	Cell* GetCell(int x, int y); 
	bool IsOccupied(int x, int y);
	int GetHeight();
	int GetWidth();
	Map* GetPrevMap();
	Map* GetNextMap();

	bool MoveActor(class Actor* actor, int x, int y);
	bool PlaceActor(class Actor* actor,int x, int y);
	bool RemoveActor(class Actor* actor);
	bool ValidPos(int x, int y);

	
private:
	Cell* cellMap[20][15];
	Map* previousMap = nullptr;
	Map* nextMap = nullptr;

	void InitMap();
	void FreeMap();
};

