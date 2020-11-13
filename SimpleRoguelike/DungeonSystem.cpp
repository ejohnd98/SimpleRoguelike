#include <stdio.h>
#include <iostream>

#include "DungeonSystem.h"
#include "ECS.h"

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr<AnimationSystem> animationSystem;

void DungeonSystem::SetDungeon(std::shared_ptr<Dungeon> dungeonData) {
	dungeon = dungeonData;
}