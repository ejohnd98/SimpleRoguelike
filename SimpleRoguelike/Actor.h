#pragma once
#include "Commands.h"
class Actor
{
public:
	Actor();
	~Actor();

	void Act(Map* map);
	void GiveCommand(Command command);
};

