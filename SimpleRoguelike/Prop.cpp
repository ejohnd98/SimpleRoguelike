#include <stdio.h>
#include <string>
#include <iostream>

#include "Prop.h"
#include "Actor.h"
#include "Commands.h"
#include "Map.h"
#include "Sprite.h"

Prop::Prop()
{
	actorSprite = new Sprite(6);
	propCommand = Command::NEXT_MAP;
}

Prop::Prop(std::string n, class Sprite* spr, Command propFunction)
{
	name = n;
	actorSprite = spr;
	propCommand = propFunction;
}


Prop::~Prop()
{
	delete actorSprite;
}

bool Prop::UseProp() {
	currentMapRef->GiveMapCommand(propCommand);
	return true;
}