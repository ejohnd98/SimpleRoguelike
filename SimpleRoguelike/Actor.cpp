#include <stdio.h>
#include <string>
#include <iostream>

#include "Actor.h"
#include "Commands.h"
#include "Map.h"
#include "Sprite.h"
#include "Prop.h"
#include "GameLog.h"


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
	currentMapRef->RemoveActor(this);
}