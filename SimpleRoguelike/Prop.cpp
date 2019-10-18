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


Prop::~Prop()
{
	delete actorSprite;
}
