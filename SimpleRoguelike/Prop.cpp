#include <stdio.h>
#include <string>
#include <iostream>

#include "Prop.h"
#include "Actor.h"
#include "Commands.h"
#include "Map.h"
#include "Sprite.h"
#include "GameLog.h"

Prop::Prop()
{
	actorSprite = new Sprite(6);
}

Prop::Prop(std::string n, int sprIndex, Command propFunction)
{
	name = n;
	actorSprite = new Sprite(sprIndex);
	propCommand = propFunction;
}

Prop::Prop(std::string n, int sprIndex, Command propFunction, bool blockMovement, bool blockSight)
{
	name = n;
	actorSprite = new Sprite(sprIndex);
	propCommand = propFunction;
	blocksMovement = blockMovement;
	blocksSight = blockSight;
}


Prop::~Prop()
{
	delete actorSprite;
}

bool Prop::UseProp(Actor* user) { //If command applies to prop, then perform action. Otherwise, pass command to Map, which passes to GameLoop
	switch (propCommand) {
	case Command::USE_DOOR:
		if (blocksMovement) {
			GameLog::instance()->AddLog(user->GetName() + " opens the " + name + ".");
			blocksMovement = false;
			blocksSight = false;
			actorSprite->SetIndex(20);
		}
		else {
			GameLog::instance()->AddLog(user->GetName() + " closes the " + name + ".");
			blocksMovement = true;
			blocksSight = true;
			actorSprite->SetIndex(18);
		}
		break;
	default:
		currentMapRef->GiveMapCommand(propCommand);
		break;
	}
	
	return true;
}

bool Prop::BlocksMovement() {
	return blocksMovement;
}
bool Prop::BlocksSight() {
	return blocksSight;
}
bool Prop::CanInteract() {
	return interactable;
}