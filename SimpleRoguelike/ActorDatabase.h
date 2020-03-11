#pragma once
#include "Actor.h"
#include <vector>
class ActorDatabase
{
public:
	std::vector<Actor*> actorList;

	void AddActor(Actor* a);
	Actor* GetActor(int i);
};

