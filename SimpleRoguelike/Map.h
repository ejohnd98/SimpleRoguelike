#include "Cell.h"
#include "Commands.h"
#include "Prop.h"
#include <list>
#pragma once
class Map
{
public:
	const int MAP_WIDTH = 60;
	const int MAP_HEIGHT = 40;
	std::list<Actor*> actorList = {};
	Cell* entrance = nullptr;
	Cell* exit = nullptr;

	Map();
	~Map();
	Cell* GetCell(int x, int y); 
	bool IsOccupied(int x, int y);
	bool IsWall(int x, int y);
	bool PathBlocked(int x, int y);
	bool IsVisible(int x, int y);
	void SetVisible(int x, int y, bool vis);
	void SetAllVisible(bool vis);
	bool IsKnown(int x, int y);
	void SetKnown(int x, int y, bool vis);
	void SetAllKnown(bool vis);
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
	Cell* cellMap[60][40];
	bool isVisible[60][40];
	bool isKnown[60][40];

	void InitMap();
	void FreeMap();
};

