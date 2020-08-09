#pragma once
#include "System.h"
#include "Commands.h"

class AISystem : public System
{
public:
	void DetermineAction();
	Entity GetTarget();
private:
	Entity currentEntity = NULL_ENTITY;
};

