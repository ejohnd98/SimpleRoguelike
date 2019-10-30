#include "Cell.h"
#include "Actor.h"
#include "Sprite.h"
#include <iostream>

//Variables:

//Functions:
Cell::Cell(int xi, int yi){
	cellSprite = new Sprite(1);
	x = xi;
	y = yi;
}
Cell::~Cell(){}

int Cell::GetX() {
	return x;
}

int Cell::GetY() {
	return y;
}

void Cell::SetupCell(bool wall) {
	isWall = wall;
	if (isWall) {
		cellSprite->SetIndex(3);
	}
}

bool Cell::IsOccupied() {
	return (isWall || ContainsActor() || ContainsProp());
}

Actor* Cell::GetActor() {
	return actor;
}

Prop* Cell::GetProp() {
	return prop;
}

Sprite* Cell::GetSprite() {
	return cellSprite;
}

bool Cell::ContainsActor() {
	return (actor != nullptr);
}

bool Cell::ContainsProp() {
	return (prop != nullptr);
}

void Cell::SetActor(Actor* a) {
	actor = a;
}

void Cell::SetProp(Prop* p) {
	prop = p;
}

void Cell::RemoveActor() {
	actor = nullptr;
}

void Cell::RemoveProp() {
	prop = nullptr;
}