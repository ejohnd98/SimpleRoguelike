#include <vector>

#include "DungeonHolder.h"
#include "Map.h"


DungeonHolder::DungeonHolder()
{
}

DungeonHolder::~DungeonHolder()
{
	for (int i = 0; i < numOfLevels; i++) {
		delete mapList[i];
	}
}

Map* DungeonHolder::GetMapAtDepth(int depth) {
	if (depth <= numOfLevels && depth > 0) {
		return mapList[depth-1];
	}
	else {
		return nullptr;
	}
}