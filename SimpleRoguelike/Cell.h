#pragma once
#include "Actor.h"
class Cell
{
public:
	Cell();
	~Cell();
	void SetupCell(bool wall);

	bool IsOccupied();
	Actor* GetActor();
	void SetActor(Actor* a);
	
private:
	bool isWall;
};

