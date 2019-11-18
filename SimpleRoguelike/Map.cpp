#include <stdio.h>
#include <iostream>
#include <list>
#include <string>
#include "Map.h"
#include "GameLoop.h"
#include "Actor.h"
#include "Prop.h"
#include "Cell.h"


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

void Map::InitMap() {

	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			cellMap[x][y] = new Cell(x,y);
			bool setWall = false;
			if (x == 0 || y == 0 || x == MAP_WIDTH-1 || y == MAP_HEIGHT-1 || (x == 7 && y != 12) || (x == 13 && y != 2)) {
				setWall = true;
			}
			cellMap[x][y]->SetupCell(setWall);
			isVisible[x][y] = false;
			isKnown[x][y] = false;
		}
	}

}

void Map::FreeMap() {

	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			delete cellMap[x][y];
		}
	}

}

Cell* Map::GetCell(int x, int y) {
	if (ValidPos(x, y)) {
		return cellMap[x][y];
	}
	else {
		return false;
	}
}

bool Map::IsOccupied(int x, int y) {
	if (ValidPos(x, y)) {
		return cellMap[x][y]->IsOccupied();
	}
	else {
		return false;
	}
}

bool Map::IsWall(int x, int y) {
	if (ValidPos(x, y)) {
		return cellMap[x][y]->IsWall();
	}
	else {
		return false;
	}
}

bool Map::PathBlocked(int x, int y) {
	if (ValidPos(x, y)) {
		return (cellMap[x][y]->IsWall() || cellMap[x][y]->ContainsProp());
	}
	else {
		return false;
	}
	
}

bool Map::IsVisible(int x, int y) {
	return isVisible[x][y];
}
void Map::SetVisible(int x, int y, bool vis) {
	isVisible[x][y] = vis;
}

void Map::SetAllVisible(bool vis) {
	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			isVisible[x][y] = vis;
		}
	}
}

bool Map::IsKnown(int x, int y) {
	return isKnown[x][y];
}
void Map::SetKnown(int x, int y, bool vis) {
	isKnown[x][y] = vis;
}

void Map::SetAllKnown(bool vis) {
	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			isKnown[x][y] = vis;
		}
	}
}

int Map::GetHeight() {
	return MAP_HEIGHT;
}
int Map::GetWidth() {
	return MAP_WIDTH;
}

void Map::GiveMapCommand(Command command) {
	gameLoop->GiveCommandFromMap(command);
}


bool Map::MoveActor(Actor* actor, int x, int y) {
	if (!cellMap[x][y]->ContainsActor() && ValidPos(x,y)) {
		cellMap[actor->GetX()][actor->GetY()]->RemoveActor();
		cellMap[x][y]->SetActor(actor);
		actor->SetPos(x, y);
		return true;
	}
	return false;
}
bool Map::PlaceActor(Actor* actor, int x, int y) {
	if (!cellMap[x][y]->ContainsActor() && ValidPos(x, y)) {
		cellMap[x][y]->SetActor(actor);
		actorList.push_back(actor);
		actor->SetPos(x, y);
		actor->SetMapRef(this);
		return true;
	}
	return false;
}
bool Map::RemoveActor(Actor* actor, bool deallocate) {
	cellMap[actor->GetX()][actor->GetY()]->RemoveActor();
	actorList.remove(actor);
	actor->SetMapRef(nullptr);
	if (deallocate) {
		delete actor;
	}
	return true;
}

bool Map::MoveProp(Prop* prop, int x, int y) {
	if (!cellMap[x][y]->ContainsProp() && ValidPos(x, y)) {
		cellMap[prop->GetX()][prop->GetY()]->RemoveProp();
		cellMap[x][y]->SetProp(prop);
		prop->SetPos(x, y);
		return true;
	}
	return false;
}
bool Map::PlaceProp(Prop* prop, int x, int y) {
	if (!cellMap[x][y]->ContainsProp() && ValidPos(x, y)) {
		cellMap[x][y]->SetProp(prop);
		prop->SetPos(x, y);
		prop->SetMapRef(this);
		return true;
	}
	return false;
}
bool Map::RemoveProp(Prop* prop) {
	cellMap[prop->GetX()][prop->GetY()]->RemoveProp();
	prop->SetMapRef(nullptr);
	delete prop;
	return true;
}

bool Map::ValidPos(int x, int y) {
	return (x >= 0 && x < MAP_WIDTH) && (y >= 0 && y < MAP_HEIGHT);
}

void Map::SetGameLoop(GameLoop* gl) {
	gameLoop = gl;
}