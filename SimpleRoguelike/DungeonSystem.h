#pragma once
#include "System.h"

class DungeonSystem : public System
{
public:
	void SetDungeon(std::shared_ptr<Dungeon> dungeonData);

	std::shared_ptr<Dungeon> dungeon;
private:

};