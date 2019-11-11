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
			if (x == 0 || y == 0 || x == MAP_WIDTH-1 || y == MAP_HEIGHT-1) {
				setWall = true;
			}
			cellMap[x][y]->SetupCell(setWall);
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
	return cellMap[x][y];
}

bool Map::IsOccupied(int x, int y) {
	return cellMap[x][y]->IsOccupied();
}

bool Map::IsWall(int x, int y) {
	return cellMap[x][y]->IsWall();
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