#include <stdio.h>
#include <iostream>
#include <vector>

#include "DungeonGenerator.h"
#include "DungeonHolder.h"
#include "MapGenerator.h"
#include "Map.h"
#include "Prop.h"


DungeonGenerator::DungeonGenerator()
{
	mapGen = new MapGenerator();
}


DungeonGenerator::~DungeonGenerator()
{
	delete mapGen;
}


DungeonHolder* DungeonGenerator::GenerateDungeon(int depth) {
	DungeonHolder* dungeon = new DungeonHolder();
	dungeon->numOfLevels = depth;
	dungeon->mapList.resize(depth, nullptr);

	for (int d = 0; d < depth; d++) { //generate starting from first level
		dungeon->mapList[d] = mapGen->GenerateNewMap(30,30);

		/*hardcode for now:
		if (d != depth - 1) {
			Prop* exit = new Prop("Exit", 17, Command::NEXT_MAP);
			if (dungeon->mapList[d]->PlaceProp(exit, 2, 2)) {
				std::cout << "Placed " << exit->GetName() << " succesfully" << "\n";
			}
			dungeon->mapList[d]->exit = dungeon->mapList[d]->GetCell(2, 2);
		}
		if (d != 0) {
			Prop* exit = new Prop("Entrance", 16, Command::PREV_MAP);
			if (dungeon->mapList[d]->PlaceProp(exit, 8, 8)) {
				std::cout << "Placed " << exit->GetName() << " succesfully" << "\n";
			}
			dungeon->mapList[d]->entrance = dungeon->mapList[d]->GetCell(8, 8);
		}*/
	}
	
	return dungeon;
}