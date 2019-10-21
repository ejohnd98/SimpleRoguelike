#include "Cell.h"
#include "Commands.h"
#include "Prop.h"
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

	void GiveMapCommand(Command command);

	bool MoveActor(class Actor* actor, int x, int y);
	bool PlaceActor(class Actor* actor,int x, int y);
	bool RemoveActor(class Actor* actor, bool deallocate = true);

	bool MoveProp(class Prop* prop, int x, int y);
	bool PlaceProp(class Prop* prop, int x, int y);
	bool RemoveProp(class Prop* prop);

	bool ValidPos(int x, int y);
	void SetGameLoop(class GameLoop* gl);
	
private:
	class GameLoop* gameLoop = nullptr;
	Cell* cellMap[20][15];
	Map* previousMap = nullptr;
	Map* nextMap = nullptr;

	void InitMap();
	void FreeMap();
};

