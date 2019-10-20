#include <vector>

#include "DungeonHolder.h"
#include "Map.h"


DungeonHolder::DungeonHolder()
{
}


DungeonHolder::~DungeonHolder()
{
}

Map* DungeonHolder::GetMapAtDepth(int depth) {
	if (depth < numOfLevels && depth >= 0) {
		return mapList[depth];
	}
	else {
		return nullptr;
	}
}