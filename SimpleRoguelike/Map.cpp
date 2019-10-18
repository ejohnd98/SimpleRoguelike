#include <stdio.h>
#include <iostream>
#include <list>
#include <string>
#include "Map.h"
#include "Actor.h"
#include "Prop.h"
#include "Cell.h"
#include "GameLoop.h"


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
			cellMap[x][y] = new Cell();
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

int Map::GetHeight() {
	return MAP_HEIGHT;
}
int Map::GetWidth() {
	return MAP_WIDTH;
}

Map* Map::GetPrevMap() {
	if (previousMap != nullptr) {
		return previousMap;
	}
	return nullptr;
}
Map* Map::GetNextMap() {
	if (nextMap != nullptr) {
		return nextMap;
	}
	return nullptr;
}

void Map::GiveMapCommand(Command command) {

	switch (command) {
		case Command::NEXT_MAP:
			std::cout << "Map received NEXT_MAP command\n";
			gameLoop->ChangeMap(GetNextMap());
		case Command::PREV_MAP:
			std::cout << "Map received PREV_MAP command\n";
			gameLoop->ChangeMap(GetPrevMap());
	}
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
		return true;
	}
	return false;
}
bool Map::RemoveProp(Prop* prop) {
	cellMap[prop->GetX()][prop->GetY()]->RemoveProp();
	delete prop;
	return true;
}


bool Map::ValidPos(int x, int y) {
	return (x >= 0 && x < MAP_WIDTH) && (y >= 0 && y < MAP_HEIGHT);
}

void Map::SetGameLoop(GameLoop* gl) {
	gameLoop = gl;
}