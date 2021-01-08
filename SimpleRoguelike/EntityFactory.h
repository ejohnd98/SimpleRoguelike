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
	int inventorySize = 0;
};

struct PropType {
	std::string name = "Empty";
	std::string description = "Empty";
	PropFunction function = PropFunction::NONE;
	Sprite closedSprite = 0;
	Sprite openedSprite = 0;
};

static std::vector<ActorType> actorTypes{
	{"Player", "you", {50, 5, 5, 1, 1, 10, 8}, 32, {32,33}, {40,40}, 10},
	{"Ghost", "boo", {5, 6, 2, 1, 1, 20, 6}, 34, {34,35}, {25,25}, 5},
	{"Skeleton", "bones", {10, 6, 3, 1, 1, 5, 6}, 37, {37,38}, {60,60}, 5}
};

static std::vector<PropType> propTypes{
	{"Door", "You can open and close it.", PropFunction::DOOR, 18, 20},
	{"Stairs Down", "Go deeper into the dungeon.", PropFunction::STAIRS_DOWN, 17, 17},
	{"Stairs Up", "Retreat to a higher floor.", PropFunction::STAIRS_UP, 16, 16}
};

class EntityFactory
{
public:
	Entity CreateActor(ActorType type, bool playerControlled = false);
	Entity CreateProp(PropType type);
private:
	Entity CreateMapEntity(std::string name, std::string desc, Sprite sprite);
};

