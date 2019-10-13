#include "Cell.h"
#include "Actor.h"
#include "Sprite.h"
#include <iostream>

//Variables:

//Functions:
Cell::Cell(){
	cellSprite = new Sprite(0);
}
Cell::~Cell(){}

void Cell::SetupCell(bool wall) {
	isWall = wall;
	if (isWall) {
		cellSprite->SetIndex(1);
	}
}

bool Cell::IsOccupied() {
	return (isWall || ContainsActor());
}

Actor* Cell::GetActor() {
	return actor;
}

Sprite* Cell::GetSprite() {
	return cellSprite;
}

bool Cell::ContainsActor() {
	return (actor != nullptr);
}

void Cell::SetActor(Actor* a) {
	actor = a;
}

void Cell::RemoveActor() {
	actor = nullptr;
}