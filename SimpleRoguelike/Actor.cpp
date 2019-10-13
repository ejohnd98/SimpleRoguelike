#include <stdio.h>
#include <string>

#include "Actor.h"
#include "Commands.h"
#include "Map.h"
#include "Sprite.h"



Actor::Actor() {
	actorSprite = new Sprite(2);
}
Actor::~Actor() {}

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

int Actor::GetX() {
	return x;
}
int Actor::GetY() {
	return y;
}

Sprite* Actor::GetSprite() {
	return actorSprite;
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

//Private:
void Move(int x, int y) {
	//if (currentMapRef->IsOccupied(x, y)) {

	//}
	return;
}