#pragma once
#include "Actor.h"
class Prop: public Actor
{
public:
	Prop();
	Prop(std::string n, int sprIndex, Command propFunction);
	~Prop();
	bool UseProp();

private:
	Command propCommand = Command::NONE;
};
