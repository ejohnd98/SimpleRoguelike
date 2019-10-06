#pragma once
#include "Commands.h"
class Actor
{
public:
	Actor();
	~Actor();

	void Act();
	void GiveCommand(Command command);
};

