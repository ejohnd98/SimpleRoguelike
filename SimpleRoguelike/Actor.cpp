#include <stdio.h>
#include <string>
#include <iostream>
#include <math.h>

#include "Actor.h"
#include "Commands.h"
#include "Map.h"
#include "Sprite.h"
#include "Prop.h"
#include "GameLog.h"
#include "Pathfinder.h"
#include "FieldOfView.h"


Actor::Actor() {
	actorSprite = new Sprite(2);
}
Actor::Actor(std::string n, int sprIndex)
{
	name = n;
	actorSprite = new Sprite(sprIndex);
}
Actor::~Actor() {
	delete actorSprite;
}

//Public:
void Actor::Act() {
	//std::cout << "Act called on: " << name << "\n";

	//TO-DO:
	//To get target, get all actors on map within sight distance
	//Then, perform a raycast (through FOV script) to see if it is on actor's FOV
	//Finally, cycle through resulting actors (if any) and decide somehow (closest dist to start)
	Actor* target = nullptr;
	switch (state) {
	case ActorState::IDLE:
		break;
	case ActorState::ATTACK:
		target = DetermineTarget();
		if (target != nullptr) {
			Pathfinder::Coordinate coord = Pathfinder::GetPath(x, y, target->GetX(), target->GetY(), currentMapRef);
			Command command = CoordToMoveCommand(coord.x, coord.y);
			GiveCommand(command);
		}
		break;
	case ActorState::FLEE:
		break;
	}
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
	case Command::WAIT:
		return true;
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
int Actor::GetFaction() {
	return faction;
}
int Actor::GetSight() {
	return sight;
}
int Actor::GetHealth() {
	return health;
}
void Actor::SetFaction(int newFaction) {
	faction = newFaction;
}
void Actor::SetMapRef(Map* mapRef) {
	currentMapRef = mapRef;
}

//Private:
bool Actor::Move(int x, int y) {
	if (currentMapRef->IsOccupied(x, y)) {
		return ActOnOther(x, y);
	}
	else {
		return currentMapRef->MoveActor(this, x, y);
	}
}

bool Actor::ActOnOther(int x, int y) {
	if (currentMapRef->GetCell(x,y)->ContainsActor()) {
		Actor* actor = currentMapRef->GetCell(x, y)->GetActor();
		//if actor is enemy:
		int attackPower = strength; //getAttackPower()
		actor->AttackUpon(attackPower, this);
		return true;
	}
	if (currentMapRef->GetCell(x, y)->ContainsProp()) {
		Prop* prop = currentMapRef->GetCell(x, y)->GetProp();
		std::cout << name << " acts upon: " << prop->GetName() << "\n";
		GameLog::instance()->AddLog(name + " acts upon " + prop->GetName());
		prop->UseProp();
		return true;
	}
	return false;
}

void Actor::AttackUpon(int attackPower, Actor* attacker) {
	int damageToDeal = attackPower; //convert attack power to damage function (armor and stuff)
	GameLog::instance()->AddLog(attacker->GetName() + " attacks " + name + " for " + std::to_string(damageToDeal) + " damage!");
	DealDamage(damageToDeal);
}

void Actor::DealDamage(int damage) {
	health -= damage;
	if (health <= 0) {
		health = 0;
		Kill();
	}
}

void Actor::Kill() {
	GameLog::instance()->AddLog(name + " has died");
	if (playerControlled) {
		currentMapRef->GiveMapCommand(Command::PLAYER_DIED);
		return;
	}
	currentMapRef->RemoveActor(this);
}

Actor* Actor::DetermineTarget() {
	std::list<Actor*> visibleActors = currentMapRef->actorList;
	Actor* target = nullptr;
	int minDist = sight; //don't target anything past max sight
	for (Actor* a : visibleActors) {
		if (a == this || a->faction == faction) { //don't attack allies or self
			continue;
		}
		if (!FieldOfView::CanSeeCell(currentMapRef, this, a->GetX(), a->GetY())) { //only target visible enemies
			continue;
		}
		int dist = abs(a->GetX() - x) + abs(a->GetY() - y);
		if (dist < minDist && dist <= sight) {
			minDist = dist;
			target = a;
		}
		return target;
	}
}

Command Actor::CoordToMoveCommand(int x2, int y2) {
	if (x < x2) {
		return Command::MOVE_RIGHT;
	}else if (x > x2) {
		return Command::MOVE_LEFT;
	}else if (y < y2) {
		return Command::MOVE_DOWN;
	}else if (y > y2) {
		return Command::MOVE_UP;
	}else {
		return Command::NONE;
	}
}