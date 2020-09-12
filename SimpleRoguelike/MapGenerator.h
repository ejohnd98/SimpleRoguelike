#pragma once
#include "System.h"
#include "RandomUtil.h"

class MapGenerator
{
public:
	MapGenerator(int seed);
	void Begin(std::shared_ptr<Map> mapData, int w, int h);
	void GenerationStep();
	void FinishMap();
	bool IsFinished();

private:
	std::shared_ptr<Map> map;
	std::vector<Position> doorPositions;
	bool isFinished = false;

	std::vector<struct RoomPrefab> roomPrefabs;
	LayoutInfo mapLayout[MAX_MAP_SIZE][MAX_MAP_SIZE];

	std::shared_ptr<RandomUtil> rand;
};

