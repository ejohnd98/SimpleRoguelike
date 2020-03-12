#include "ActorDatabase.h"
#include "Actor.h"
#include <iostream>

void ActorDatabase::AddActor(Actor* a) {
	actorList.push_back(a);
}

Actor* ActorDatabase::GetActor(int i) {
	if (i >= 0 && i < actorList.size() && actorList[i]!=nullptr) {
		Actor* copy = actorList[i]->GetCopy();
		return copy;
	}
	else {
		std::cout << "ERROR: No actor with index: " << i << ".\n";
		return new Actor();
	}
}
