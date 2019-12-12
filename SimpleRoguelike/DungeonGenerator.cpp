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

		//populate dungeon with stairs between levels
		mapGen->PopulateDungeon(d, depth, dungeon->mapList[d]);
	}
	
	return dungeon;
}