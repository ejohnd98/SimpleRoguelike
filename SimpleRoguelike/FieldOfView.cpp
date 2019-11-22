#include <iostream>
#include <math.h> 

#include "FieldOfView.h"
#include "Actor.h"
#include "Map.h"
#include "Cell.h"

int playerX, playerY, sightDist;
Map* map;
void SendRay(float x, float y);
float GetDist(int x1, int y1, int x2, int y2);

void FieldOfView::SetFOV(Map* m, Actor* p) {
	map = m;
	playerX = p->GetX();
	playerY = p->GetY();
	sightDist = p->GetSight();
	map->SetAllVisible(false);
	float x, y;
	for (int a = 0; a < 360; a++) {
		x = cos(0.01745f * (float)a);
		y = sin(0.01745f * (float)a);
		SendRay(x, y);
	}
}

void SendRay(float x, float y){
	float ox, oy;
	ox = (float)playerX + 0.5f;
	oy = (float)playerY + 0.5f;
	for (int i = 0; i <= sightDist + 1; i++){
		if (!map->ValidPos((int)ox, (int)oy)) {
			return;
		}
		map->SetVisible((int)ox, (int)oy, true);
		map->SetKnown((int)ox, (int)oy, true);
		if (map->IsWall((int)ox, (int)oy)) {
			return;
		}
		ox += x;
		oy += y;
	}
}

bool FieldOfView::InSight(int x1, int y1, int x2, int y2, int sight) {
	float dx = (x2 - x1);
	float dy = (y2 - y1);
	float magnitude = abs(dx) + abs(dy);
	dx = dx/magnitude; //normalize
	dy = dy/magnitude;
	float ox1 = x1 + 0.5f + dx * 0.25f;
	float oy1 = y1 + 0.5f + dy * 0.25f;
	float ox2 = x1 + 0.5f - dx * 0.25f;
	float oy2 = y1 + 0.5f - dy * 0.25f;
	for (int i = 0; i <= magnitude+1; i++) {
		//std::cout << "Looking: " << ox << ", " << oy << "\n";
		if (map->IsWall((int)ox1, (int)oy1) && map->IsWall((int)ox2, (int)oy2)) {
			return false;
		}
		if ((int)ox1 == x2 && (int)oy1 == y2 || (int)ox2 == x2 && (int)oy2 == y2) {
			//std::cout << "Found\n";
			return true;
		}
		ox1 += dx;
		oy1 += dy;
		ox2 += dx;
		oy2 += dy;
	}
	return false;
}

bool FieldOfView::CanSeeCell(Map* m, Actor* s, int x, int y) {
	if (GetDist(s->GetX(), s->GetY(), x, y) > s->GetSight()) {
		return false;
	}
	if (s->GetX() == x && s->GetY() == y) {
		return true;
	}
	map = m;
	return InSight(s->GetX(), s->GetY(), x, y, s->GetSight());
}

float GetDist(int x1, int y1, int x2, int y2) {
	int x = x2 - x1;
	int y = y2 - y1;
	return sqrt((x * x) + (y * y));
}