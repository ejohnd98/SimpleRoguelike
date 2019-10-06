#include "Cell.h"
#include "Actor.h"
#include <iostream>

//Variables:
Actor* actor = NULL;

//Functions:
Cell::Cell(){}
Cell::~Cell(){}

void Cell::SetupCell(bool wall) {
	isWall = wall;
}

bool Cell::IsOccupied() {
	return isWall;
}

Actor* Cell::GetActor() {
	return actor;
}

void Cell::SetActor(Actor* a) {
	actor = a;
}