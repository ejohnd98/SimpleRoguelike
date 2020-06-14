#include <vector>

#include "FieldOfView.h"
#include "ECS.h"

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr <MapSystem> mapSystem;

void FieldOfView::CalculateVisibleCells(Entity e) {

}

std::vector<Entity> FieldOfView::GetVisibleEntities(Entity e) {
	std::vector<Entity> visibleEntities;
	auto map = mapSystem->map;
	Position pos = ecs->GetComponent<Position>(e);

	for (std::pair<Position, Entity> entityPosPair : map->positionEntityMap){
		if (entityPosPair.second != e && HasLineOfSight(pos, entityPosPair.first)) {
			visibleEntities.push_back(entityPosPair.second);
		}
	}
	return visibleEntities;
}
bool FieldOfView::HasLineOfSight(Position a, Position b) {
	//auto map = mapSystem->map;
	int sight = 1000;
	float x, y, dx, dy, ox, oy, stepDist;
	int lastX, lastY, dirXMod, dirYMod; //used for diagonal wall checking (an edge case)
	dx = (b.x - a.x); //x dist between two cells
	dy = (b.y - a.y); //x dist between two cells
	dirXMod = dx / abs(dx); //sets to a value of 1 with sign depending on direction of ray (-1 or 1)
	dirYMod = dy / abs(dy);

	if (abs(dx) == abs(dy)) { //normalize direction (offsets will be consistent regardless of dx and dy)
		ox = dx / abs(dx);
		oy = dy / abs(dy);
	}
	else if (abs(dx) > abs(dy)) {
		ox = 0.5f * (dx / abs(dx));
		oy = 0.5f * (dy / abs(dx));
	}
	else {
		ox = 0.5f * (dx / abs(dy));
		oy = 0.5f * (dy / abs(dy));
	}

	stepDist = sqrt((ox * ox) + (oy * oy));
	x = (float)a.x + 0.5f; //set initial ray position
	y = (float)a.y + 0.5f;
	lastX = x;
	lastY = y;
	for (int i = 0; i * stepDist <= sight; i++) {
		if (mapSystem->IsWall({ (int)x, (int)y })) { //return if reached wall or invalid position (outside map)
			return false;
		}
		if (((int)x) == lastX + dirXMod && ((int)y) == lastY + dirYMod) { //if moved diagonally, check for diagonal walls
			if (mapSystem->IsWall({ lastX + dirXMod, lastY }) && mapSystem->IsWall({ lastX, lastY + dirYMod })) {
				//return false; //incorrectly passed through diagonal wall, so return
			}
		}
		if (((int)x) == b.x && ((int)y) == b.y) { //found target cell, so InSight is true
			return true;
		}
		lastX = x;
		lastY = y;
		//increment ray position
		x += ox;
		y += oy;
	}
}