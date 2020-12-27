#include <stdio.h>
#include <iostream>

#include "DungeonSystem.h"
#include "ECS.h"

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr<MapSystem> mapSystem;
extern std::shared_ptr<PlayerSystem> playerSystem;

void DungeonSystem::SetDungeon(std::shared_ptr<Dungeon> dungeonData) {
	dungeon = dungeonData;
}

void DungeonSystem::GoToFloorIndex(int floor) {
	Entity player = playerSystem->GetPlayerEntity();
	if (player == NULL_ENTITY || floor < 0 || floor >= dungeon->floorCount) {
		return;
	}
	mapSystem->RemoveEntity(player);

	bool descending = (floor - currentFloor >= 0);
	currentFloor = floor;
	mapSystem->SetMap(dungeon->floors[currentFloor]);
	Position newPos;
	if (descending) {
		newPos = dungeon->floors[currentFloor]->entrance + Position{1,0};
	}
	else {
		newPos = dungeon->floors[currentFloor]->exit + Position{ -1,0 };
	}
	mapSystem->PlaceEntity(player, newPos);
	mapSystem->ClearVisible(true);
}