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

	for (int d = 0; d < depth; d++) { //generate from top to bottom (player starts at top)
		dungeon->mapList[d] = mapGen->GenerateMap(10,10);
		//hardcode for now:
		if (d != depth - 1) {
			Prop* exit = new Prop("Stairs", 6, Command::NEXT_MAP);
			if (dungeon->mapList[d]->PlaceProp(exit, 2, 2)) {
				std::cout << "Placed " << exit->GetName() << " succesfully" << "\n";
			}
		}
		if (d != 0) {
			Prop* exit = new Prop("Stairs Up", 6, Command::PREV_MAP);
			if (dungeon->mapList[d]->PlaceProp(exit, 8, 8)) {
				std::cout << "Placed " << exit->GetName() << " succesfully" << "\n";
			}
		}
		std::string name = "Ghost ";
		name += std::to_string(d+1);
		Actor* ghost = new Actor(name, 3);
		if (dungeon->mapList[d]->PlaceActor(ghost, 5, 3)) {
			std::cout << "Placed " << ghost->GetName() << " succesfully" << "\n";
		}
	}
	
	return dungeon;
}