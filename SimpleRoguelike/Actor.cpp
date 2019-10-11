#include <stdio.h>
#include "Actor.h"
#include "Commands.h"
#include "Map.h"

Actor::Actor(){}
Actor::~Actor(){}

Map* currentMapRef = nullptr;
int x, int y;

//Public:
void Actor::Act(Map* map) {
	return;
}
void Actor::GiveCommand(Command command ) {
	switch (command) {
	case Command::MOVE_UP:
		break;
	case Command::MOVE_DOWN:
		break;
	case Command::MOVE_RIGHT:
		break;
	case Command::MOVE_LEFT:
		break;
	}
}

//Private:
void Move(int x, int y) {
	if (currentMapRef->IsOccupied(x, y)) {

	}
	return;
}