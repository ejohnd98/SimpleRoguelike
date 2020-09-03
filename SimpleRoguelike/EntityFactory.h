#pragma once
#include "System.h"
#include "Commands.h"

struct ActorType {
	std::string name = "Empty";
	std::string description = "Empty";
	Stats stats = {};
	Sprite defaultSprite = 0;
	std::vector<Sprite> idleAnim;
	std::vector<int> idleAnimlengths;
};

struct PropType {
	std::string name = "Empty";
	std::string description = "Empty";
	Sprite closedSprite = 0;
	Sprite openedSprite = 0;
};

class EntityFactory
{
public:
	Entity CreateActor(ActorType type, bool playerControlled = false);
	Entity CreateDoor(PropType type);
private:
	Entity CreateMapEntity(std::string name, std::string desc, Sprite sprite);
};

