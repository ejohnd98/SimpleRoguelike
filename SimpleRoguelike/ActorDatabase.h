#pragma once
#include "Actor.h"
#include <vector>
class ActorDatabase
{
	std::vector<Actor*> actorList;
	static ActorDatabase* instancePtr;
	ActorDatabase(){}
	
public:
	~ActorDatabase() {
		instancePtr = 0;
		for (Actor* a : actorList) {
			delete a;
		}
	}
	static ActorDatabase* instance()
	{
		if (!instancePtr)
			instancePtr = new ActorDatabase;
		return instancePtr;
	}

	void AddActor(Actor* a);
	Actor* GetActor(int i);
};


