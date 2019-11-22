#pragma once
#include "Commands.h"
class GameLoop
{
public:
	GameLoop();
	~GameLoop();
	void AdvanceLoop();
	class Map** GetMapPointer();
	void GiveInput(Command command);
	void GiveCommandFromMap(Command command);
	bool ChangeMap(class Map* newMap, bool deeper);
	class Map* GetPrevMap();
	class Map* GetNextMap();
	class Map* GetCurrentMap();
	class Actor** GetPlayerPtr();
private:
	void InitializeGame();
};

