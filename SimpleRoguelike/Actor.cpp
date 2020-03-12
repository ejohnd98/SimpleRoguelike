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
#include "Coordinate.h"


Actor::Actor() {
	actorSprite = new Sprite(2);
}
Actor::Actor(std::string n, int sprIndex)
{
	health = maxHealth;
	name = n;
	actorSprite = new Sprite(sprIndex);
}
Actor::Actor(std::string name, int sprIndex, int maxHealth, int strength, int faction, int sight) : name(name), maxHealth(maxHealth), strength(strength), faction(faction), sight(sight)
{
	health = maxHealth;
	actorSprite = new Sprite(sprIndex);
}
Actor::Actor(Actor* a)
{
	name = a->name;
	maxHealth = a->maxHealth;
	strength = a->strength;
	faction = a->faction;
	sight = a->sight;
	health = maxHealth;
	actorSprite = new Sprite(a->actorSprite->GetIndex());
}
Actor::~Actor() {
	delete actorSprite;
}

//Public:
void Actor::Act() {
	Actor* target = nullptr;
	Coordinate targetCoord;

	switch (state) {
	case ActorState::IDLE:
		break;
	case ActorState::ATTACK: //should put into its own function at somepoint
		
		target = DetermineTarget(); //try and get a visible target
		
		if (target != nullptr) { //if succeeded, set targetCoord to it
			targetCoord = Coordinate(target->GetX(), target->GetY());
			targetLastPos = targetCoord;
		}else if(targetLastPos.init){ //otherwise, go to last known target position (if exists)
			if (!targetLastPos.IsCoord(x, y)) { //check if already at coord
				targetCoord = targetLastPos;
			}else{ //if so, no longer want to consider this coord again
				targetLastPos.init = false;
			}
		}else {//couldn't find anything, and no past target, so should start idling (haven't done transitions yet)
			//std::cout << "Couldn't find a target, and no past targets\n";
			targetLastPos.init = false;
		}

		if (targetCoord.init) { //if target has been set, start pathing towards it with given coordinate
			Coordinate coord = Pathfinder::GetPath(x, y, targetCoord.x, targetCoord.y, currentMapRef);
			if (coord.init) {
				Command command = CoordToMoveCommand(coord.x, coord.y);
				GiveCommand(command);
			}else {
				targetLastPos.init = false; //current target is inaccessible, so discard last known pos (maybe too stupid, but good for now)
			}
		}
		//std::cout << "Last known coord: " << targetLastPos.x << ", " << targetLastPos.y << "\n";
		break;
	case ActorState::FLEE:
		break;
	}
	return;
}
bool Actor::GiveCommand(Command command ) {
	switch (command) {
	case Command::MOVE_UP:
		return InteractWithCell(x, y - 1);
	case Command::MOVE_DOWN:
		return InteractWithCell(x, y + 1);
	case Command::MOVE_RIGHT:
		return InteractWithCell(x + 1, y);
	case Command::MOVE_LEFT:
		return InteractWithCell(x - 1, y);
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

Actor*  Actor::GetCopy() {
	Actor* copy = new Actor(this);
	return copy;
}

//Private:
bool Actor::Move(int x, int y) {
	return currentMapRef->MoveActor(this, x, y);
	
	/*if (currentMapRef->MovementBlocked(x, y)) {
		return ActOnOther(x, y);
	}
	else {
		return currentMapRef->MoveActor(this, x, y);
	}*/
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
	else if (health > maxHealth) {
		health = maxHealth;
	}
}

void Actor::Kill() {
	GameLog::instance()->AddLog( name + " has been defeated!");
	if (playerControlled) {
		currentMapRef->GiveMapCommand(Command::PLAYER_DIED);
		return;
	}
	currentMapRef->RemoveActor(this);
}

Actor* Actor::DetermineTarget() {
	std::list<Actor*> allActors = currentMapRef->actorList;
	Actor* target = nullptr;
	int minDist = sight*2; //don't target anything past max sight
	for (Actor* a : allActors) {
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
	}
	return target;
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

bool Actor::InteractWithCell(int x, int y) { //given a cell, determines the action that should be taken on it
	Cell* cell = currentMapRef->GetCell(x, y);

	if (cell->ContainsActor()) { //if actor is in cell, act upon it
		Actor* other = cell->GetActor();
		if (other->GetFaction() != faction) {
			int attackPower = strength;
			other->AttackUpon(attackPower, this);
			return true;
		}
	}
	if (!cell->MovementBlocked()) { //if cell can be moved into, move into it
		if (Move(x, y)) {
			return true;
		}
	}
	if (cell->ContainsProp()) { //if cell contains prop, use it if interactable
		Prop* prop = cell->GetProp();
		if (prop->CanInteract()) {
			if (prop->UseProp(this)) {
				return true;
			}
		}
	}
	return false; //couldn't take any actions
}