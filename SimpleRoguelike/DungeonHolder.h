#pragma once
#include <vector>
class DungeonHolder
{
public:
	DungeonHolder();
	~DungeonHolder();
	class Map* GetMapAtDepth(int depth);
	std::vector<Map*> mapList = {};
	int numOfLevels = 0;
private:
	
};

