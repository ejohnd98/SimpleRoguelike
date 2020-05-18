#pragma once
#include "Constants.h"
#include "Components.h"

class FieldOfView
{
	public:
		void CalculateVisibleCells(Entity e);
		void GetVisibleEntities(Entity e);
		bool HasLineOfSight(Position a, Position b);
};

