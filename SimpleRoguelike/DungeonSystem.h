#pragma once
#include "System.h"

class DungeonSystem : public System
{
public:
	void SetDungeon(std::shared_ptr<Dungeon> dungeonData);
	void GoToFloorIndex(int floor);

	std::shared_ptr<Dungeon> dungeon;
	int currentFloor = 0;
private:

};