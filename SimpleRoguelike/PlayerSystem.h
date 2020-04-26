#pragma once
#include "System.h"
#include "Commands.h"

class PlayerSystem : public System
{
public:
	void Init();
	bool DetermineAction(Command command);
	Entity GetPlayerEntity();
	bool InteractWithCell(Position pos);
};

