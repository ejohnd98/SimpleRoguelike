#include <stdio.h>
#include <string>
#include <iostream>

#include "Actor.h"
#include "Commands.h"
#include "Map.h"
#include "Sprite.h"
#include "Prop.h"



Actor::Actor() {
	actorSprite = new Sprite(2);
}
Actor::~Actor() {
	delete actorSprite;
}

//Public:
void Actor::Act() {
	std::cout << "Act called on: " << name << "\n";
	return;
}
bool Actor::GiveCommand(Command command ) {
	switch (command) {
	case Command::MOVE_UP:
		return Move(x, y - 1);
	case Command::MOVE_DOWN:
		return Move(x, y + 1);
	case Command::MOVE_RIGHT:
		return Move(x + 1, y);
	case Command::MOVE_LEFT:
		return Move(x - 1, y);
	}
	return false;
}

int Actor::GetX() {
	return x;
}
int Actor::GetY() {
	return y;
}

Sprite* Actor::GetSprite() {
	return actorSprite;
}
void Actor::SetSprite(class Sprite* spr) {
	actorSprite = spr;
}
void Actor::SetPos(int x2, int y2) {
	x = x2;
	y = y2;
}
std::string Actor::GetName() {
	return name;
}
void Actor::SetName(std::string newName) {
	name = newName;
}
void Actor::SetMapRef(Map* mapRef) {
	currentMapRef = mapRef;
}

//Private:
bool Actor::Move(int x, int y) {
	if (currentMapRef->IsOccupied(x, y)) {
		return ActOn(x, y);
	}
	else {
		return currentMapRef->MoveActor(this, x, y);
	}
}

bool Actor::ActOn(int x, int y) {
	if (currentMapRef->GetCell(x,y)->ContainsActor()) {
		Actor* actor = currentMapRef->GetCell(x, y)->GetActor();
		std::cout << name << " acts upon: " << actor->GetName() << "\n";
		return true;
	}
	if (currentMapRef->GetCell(x, y)->ContainsProp()) {
		Prop* prop = currentMapRef->GetCell(x, y)->GetProp();
		std::cout << name << " acts upon: " << prop->GetName() << "\n";
		return true;
	}
	return false;
}