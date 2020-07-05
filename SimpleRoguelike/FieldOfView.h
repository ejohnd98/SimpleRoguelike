#pragma once
#include "Constants.h"
#include "Components.h"

class FieldOfView
{
	public:
		void CalculateVisibleCells(Entity e);
		void CastRay(float ex, float ey, float ox, float oy, float stepDist, int sight);
		std::vector<Entity> GetVisibleEntities(Entity e);
		bool HasLineOfSight(Position a, Position b, int sight = 1000);
};

