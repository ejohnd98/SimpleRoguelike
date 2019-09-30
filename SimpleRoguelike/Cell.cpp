#include "Cell.h"
#include <iostream>

Cell::Cell(){}
Cell::~Cell(){}

bool Cell::IsOccupied() {
	return isWall;
}

void Cell::SetupCell(bool wall) {
	isWall = wall;
}

//Entity reference
//Prop reference
//Item references
//