#include <iostream>
#include <math.h> 

#include "FieldOfView.h"
#include "Actor.h"
#include "Map.h"
#include "Cell.h"

int playerX, playerY, sightDist;
Map* map;
void SendRay(float ox, float oy);
float GetDist(int x1, int y1, int x2, int y2);

void FieldOfView::SetFOV(Map* m, Actor* p) { //sets the visible and known cells on the map, based on the given actor
	map = m;
	playerX = p->GetX();
	playerY = p->GetY();
	sightDist = p->GetSight();
	map->SetAllVisible(false); //reset visiblity
	float ox, oy; //offset values (will be added to player position incrementally to check visibility)
	for (int a = 0; a < 360; a++) {//send 360 rays in 360 degrees
		ox = cos(0.1f * (float)a);
		oy = sin(0.1f * (float)a);
		SendRay(ox, oy);
	}
}

void SendRay(float ox, float oy){ //casts a ray in the direction of the parameters (x and y offsets to be applied each step)
	float x, y, stepDist;
	int lastX, lastY, dirXMod, dirYMod; //used for checking diagonal walls (an edge case where players see through 2 walls at a diagonal)
	dirXMod = ox / abs(ox); //sets to a value of 1 with sign depending on direction of ray (-1 or 1)
	dirYMod = oy / abs(oy);
	stepDist = sqrt((ox * ox) + (oy * oy)); //used to restrict ray cast distance
	x = (float)playerX + 0.5f; //initial position to start from
	y = (float)playerY + 0.5f;
	lastX = x;
	lastY = y;
	for (int i = 0; i * stepDist <= sightDist;i++) { //increment ray position until reached sight limit
		bool isWall = false;
		if (!map->ValidPos((int)x, (int)y)) { //return if reached an invalid position (outside map)
			return;
		}
		if (map->IsWall((int)x, (int)y)) {
			isWall = true;
		}
		if (((int)x) == lastX + dirXMod && ((int)y) == lastY + dirYMod) { //if moved diagonally, check for diagonal walls
			if (map->IsWall(lastX + dirXMod, lastY) && map->IsWall(lastX, lastY + dirYMod)) {
				if (isWall) { //still want to see walls placed in corners (but not open cells)
					map->SetVisible((int)x, (int)y, true);
					map->SetKnown((int)x, (int)y, true);
				}
				return; //current cell is through a diagonal wall, so stop ray
			}
		}
		//make current space visible
		map->SetVisible((int)x, (int)y, true);
		map->SetKnown((int)x, (int)y, true);
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

bool FieldOfView::InSight(int x1, int y1, int x2, int y2, int sight) { //casts a ray from pos 1 to pos 2 to see if they can see eachother
 	float x, y, dx, dy, ox, oy, stepDist;
	int lastX, lastY, dirXMod, dirYMod; //used for diagonal wall checking (an edge case)
	dx = (x2 - x1); //x dist between two cells
	dy = (y2 - y1); //x dist between two cells
	dirXMod = dx / abs(dx); //sets to a value of 1 with sign depending on direction of ray (-1 or 1)
	dirYMod = dy / abs(dy);

	if (abs(dx) == abs(dy)) { //normalize direction (offsets will be consistent regardless of dx and dy)
		ox = dx / abs(dx);
		oy = dy / abs(dy);
	}else if (abs(dx) > abs(dy)) {
		ox = 0.5f * (dx / abs(dx));
		oy = 0.5f * (dy / abs(dx));
	}else {
		ox = 0.5f * (dx / abs(dy));
		oy = 0.5f * (dy / abs(dy));
	}

	stepDist = sqrt((ox * ox) + (oy * oy));
	x = (float)x1 + 0.5f; //set initial ray position
	y = (float)y1 + 0.5f;
	lastX = x;
	lastY = y;
	for (int i = 0; i * stepDist <= sight; i++) {
		if (!map->ValidPos((int)x, (int)y) || map->IsWall((int)x, (int)y)) { //return if reached wall or invalid position (outside map)
			return false;
		}
		if (((int)x) == lastX + dirXMod && ((int)y) == lastY + dirYMod) { //if moved diagonally, check for diagonal walls
			if (map->IsWall(lastX + dirXMod, lastY) && map->IsWall(lastX, lastY + dirYMod)) {
				return false; //incorrectly passed through diagonal wall, so return
			}
		}
		if (((int)x) == x2 && ((int)y) == y2) { //found target cell, so InSight is true
			return true;
		}
		lastX = x;
		lastY = y;
		//increment ray position
		x += ox;
		y += oy;
	}
}

bool FieldOfView::CanSeeCell(Map* m, Actor* s, int x, int y) { //wrapper function for InSight called by Actors
	if (GetDist(s->GetX(), s->GetY(), x, y) > s->GetSight()) { //check if cell is too far away to see
		return false;
	}
	if (s->GetX() == x && s->GetY() == y) { //check if position is the same
		return true;
	}
	map = m;
	return InSight(s->GetX(), s->GetY(), x, y, s->GetSight()); //otherwise cast a ray to check if target cell is visible
}

float GetDist(int x1, int y1, int x2, int y2) { //uses simple trig to get distance between two points
	int x = x2 - x1;
	int y = y2 - y1;
	return sqrt((x * x) + (y * y));
}