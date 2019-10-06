#include "Actor.h"
#include "Commands.h"

Actor::Actor(){}
Actor::~Actor(){}

//Public:
void Actor::Act() {
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
	return;
}