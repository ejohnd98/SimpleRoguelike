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
}

Prop::Prop(std::string n, int sprIndex, Command propFunction)
{
	name = n;
	actorSprite = new Sprite(sprIndex);
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