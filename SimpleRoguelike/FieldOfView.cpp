#include <vector>
#include <math.h> 

#include "FieldOfView.h"
#include "ECS.h"

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr <MapSystem> mapSystem;

#define PI 3.14159265

void FieldOfView::CalculateVisibleCells(Entity e) {
	Stats& stats = ecs->GetComponent<Stats>(e);
	FloatPosition pos = ecs->GetComponent<Renderable>(e).position;
	mapSystem->ClearVisible();
	float ox, oy; //offset values (will be added to player position incrementally to check visibility)
	float stepDist = 0.025f;
	for (float a = 0; a < 360; a += 0.5f) {//sends 720 rays in 360 degrees (0.5 degrees each loop)
		ox = stepDist * cos(PI * a / 180.0f);
		oy = stepDist * sin(PI * a / 180.0f);
		CastRay(pos.x, pos.y, ox, oy, stepDist, stats.sight);
	}
}

void FieldOfView::CastRay(float ex, float ey, float ox, float oy, float stepDist, int sight) {
	float x, y;
	int lastX, lastY, dirXMod, dirYMod; //used for checking diagonal walls (an edge case where players see through 2 walls at a diagonal)
	dirXMod = ox / abs(ox); //sets to a value of 1 with sign depending on direction of ray (-1 or 1)
	dirYMod = oy / abs(oy);
	x = (float)ex + 0.5f; //initial position to start from
	y = (float)ey + 0.5f;
	lastX = x;
	lastY = y;
	for (int i = 0; i * stepDist <= sight; i++) { //increment ray position until reached sight limit
		bool isWall = false;
		if (!mapSystem->ValidPosition({ (int)x, (int)y })) { //return if reached an invalid position (outside map)
			return;
		}
		if (mapSystem->BlocksSight({ (int)x, (int)y })) {
			isWall = true;
		}
		if (((int)x) == lastX + dirXMod && ((int)y) == lastY + dirYMod) { //if moved diagonally, check for diagonal walls
			if (mapSystem->BlocksSight({ lastX + dirXMod, lastY }) && mapSystem->BlocksSight({ lastX, lastY + dirYMod })) {
				if (isWall) { //still want to see walls placed in corners (but not open cells)
					mapSystem->SetVisible((int)x, (int)y);
					mapSystem->SetKnown((int)x, (int)y);
				}
				return; //current cell is through a diagonal wall, so stop ray
			}
		}
		//make current space visible
		mapSystem->SetVisible((int)x, (int)y);
		mapSystem->SetKnown((int)x, (int)y);
		if (isWall) { //stop ray if current cell is a wall
			return;
		}
		lastX = x;
		lastY = y;
		//increment ray position
		x += ox;
		y += oy;
	}
}

std::vector<Entity> FieldOfView::GetVisibleEntities(Entity e) {
	std::vector<Entity> visibleEntities;
	auto map = mapSystem->map;
	Position pos = ecs->GetComponent<Position>(e);
	Stats& stats = ecs->GetComponent<Stats>(e);

	for (std::pair<Position, Entity> entityPosPair : map->positionEntityMap){
		if (entityPosPair.second != e && HasLineOfSight(pos, entityPosPair.first, stats.sight)) {
			visibleEntities.push_back(entityPosPair.second);
		}
	}
	return visibleEntities;
}

bool FieldOfView::HasLineOfSight(Position a, Position b, int sight) {
	if (a.Dist(b) > sight + 2) {
		return false;
	}
	float x, y, dx, dy, ox, oy, stepDist;
	int lastX, lastY, dirXMod, dirYMod; //used for diagonal wall checking (an edge case)
	dx = (float)(b.x - a.x); //x dist between two cells
	dy = (float)(b.y - a.y); //x dist between two cells
	dirXMod = (int)(dx / abs(dx)); //sets to a value of 1 with sign depending on direction of ray (-1 or 1)
	dirYMod = (int)(dy / abs(dy));

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
		if (mapSystem->BlocksSight({ (int)x, (int)y })) { //return if reached wall or invalid position (outside map)
			return false;
		}
		if (((int)x) == lastX + dirXMod && ((int)y) == lastY + dirYMod) { //if moved diagonally, check for diagonal walls
			if (mapSystem->BlocksSight({ lastX + dirXMod, lastY }) && mapSystem->BlocksSight({ lastX, lastY + dirYMod })) {
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
	return false;
}