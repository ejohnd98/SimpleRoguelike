#pragma once
#include "System.h"
#include "Commands.h"

class InteractionHandler : public System
{
public:
	bool PerformAction(Entity entity, Position target, InteractType interactType);

};