#pragma once
#include "Constants.h"
#include "Components.h"

class FieldOfView
{
	public:
		void CalculateVisibleCells(Entity e);
		std::vector<Entity> GetVisibleEntities(Entity e);
		bool HasLineOfSight(Position a, Position b);
};

