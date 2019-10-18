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
	bool ChangeMap(Map* newMap);
private:
	void InitializeGame();
};

