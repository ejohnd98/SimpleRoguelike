#pragma once
#include "Actor.h"
class Prop: public Actor
{
public:
	Prop();
	~Prop();
private:
	Command propCommand = Command::NONE;
};

