#include "Cell.h"
#include "Actor.h"
#include "Prop.h"
#include "Sprite.h"
#include <iostream>

//Variables:

//Functions:
Cell::Cell(int xi, int yi){
	cellSprite = new Sprite(0);
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
	else {
		cellSprite->SetIndex(1);
	}
}

bool Cell::MovementBlocked(bool ignoreActors) {
	if (isWall) {
		return true;
	}
	if (ContainsProp() && prop->BlocksMovement()) {
		return true;
	}
	return false;
}

bool Cell::SightBlocked() {
	if (isWall) {
		return true;
	}
	if (ContainsProp() && prop->BlocksSight()) {
		return true;
	}
	return false;
}

bool Cell::IsWall() {
	return isWall;
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