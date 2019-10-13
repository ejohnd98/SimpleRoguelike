#include <stdio.h>
#include <list>
#include "Map.h"
#include "Actor.h"
#include "Cell.h"

const int MAP_WIDTH = 20;
const int MAP_HEIGHT = 15;

Cell* cellMap[MAP_WIDTH][MAP_HEIGHT];

void InitMap();
void FreeMap();

Map::Map() {
	InitMap();
}

Map::~Map() {
	FreeMap();
	for (Actor* a : actorList) {
		delete a;
	}
	actorList.clear();
}

void InitMap() {

	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			cellMap[x][y] = new Cell();
			bool setWall = false;
			if (x == 0 || y == 0 || x == MAP_WIDTH-1 || y == MAP_HEIGHT-1) {
				setWall = true;
			}
			cellMap[x][y]->SetupCell(setWall);
		}
	}

}

void FreeMap() {

	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			delete cellMap[x][y];
		}
	}

}

Cell* Map::GetCell(int x, int y) {
	return cellMap[x][y];
}

bool Map::IsOccupied(int x, int y) {
	return cellMap[x][y]->IsOccupied();
}

int Map::GetHeight() {
	return MAP_HEIGHT;
}
int Map::GetWidth() {
	return MAP_WIDTH;
}
bool Map::MoveActor(class Actor* actor, int x, int y) {
	if (cellMap[x][y]->GetActor() == nullptr && ValidPos(x,y)) {
		cellMap[actor->GetX()][actor->GetY()]->RemoveActor();
		cellMap[x][y]->SetActor(actor);
		actor->SetPos(x, y);
		return true;
	}
	return false;
}
bool Map::PlaceActor(class Actor* actor, int x, int y) {
	if (cellMap[x][y]->GetActor() == nullptr && ValidPos(x, y)) {
		cellMap[x][y]->SetActor(actor);
		actorList.push_back(actor);
		actor->SetPos(x, y);
		return true;
	}
	return false;
}
bool Map::RemoveActor(Actor* actor) {
	cellMap[actor->GetX()][actor->GetY()]->RemoveActor();
	actorList.remove(actor);
	delete actor;
	return true;
}

bool Map::ValidPos(int x, int y) {
	return (x >= 0 && x < MAP_WIDTH) && (y >= 0 && y < MAP_HEIGHT);
}