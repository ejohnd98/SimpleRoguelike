#include "Cell.h"
#include "Commands.h"
#include "Prop.h"
#include <list>
#pragma once
class Map
{
public:
	const static int MAP_WIDTH = 60;
	const static int MAP_HEIGHT = 40;
	const static int MAP_CELLS = MAP_WIDTH * MAP_HEIGHT;

	std::list<Actor*> actorList = {};
	Cell* entrance = nullptr;
	Cell* exit = nullptr;

	Map(int w, int h);
	~Map();
	Cell* GetCell(int x, int y); 

	bool IsWall(int x, int y);
	bool MovementBlocked(int x, int y, bool ignoreActors = false);
	bool SightBlocked(int x, int y);

	bool IsVisible(int x, int y);
	void SetVisible(int x, int y, bool vis);
	void SetAllVisible(bool vis);
	bool IsKnown(int x, int y);
	void SetKnown(int x, int y, bool vis);
	void SetAllKnown(bool vis);

	int GetHeight();
	int GetWidth();
	void SetSize(int w, int h);
	int GetNumOfCells();
	Coordinate GetPosAroundStairs(bool entering);

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
	int width;
	int height;
	class GameLoop* gameLoop = nullptr;
	Cell* cellMap[MAP_WIDTH][MAP_HEIGHT];
	bool isVisible[MAP_WIDTH][MAP_HEIGHT];
	bool isKnown[MAP_WIDTH][MAP_HEIGHT];

	void InitMap();
	void FreeMap();
};

