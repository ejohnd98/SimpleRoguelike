#pragma once
#include "Actor.h"
class Prop: public Actor
{
public:
	Prop();
	Prop(std::string n, int sprIndex, Command propFunction);
	Prop(std::string n, int sprIndex, Command propFunction, bool blockMovement, bool blockSight);
	~Prop();
	bool UseProp(Actor* user);
	bool BlocksMovement();
	bool BlocksSight();
	bool CanInteract();

private:
	Command propCommand = Command::NONE;
	bool blocksMovement = true;
	bool blocksSight = false;
	bool interactable = true;
};
