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
	bool IsStarted();
	bool IsFinished();

	std::shared_ptr<Map> map;
	LayoutInfo mapLayout[MAX_MAP_SIZE][MAX_MAP_SIZE];
	std::vector<Position> possibleDoorPositions;
	std::vector<Position> doorPositions;

private:
	bool RoomFits(RoomPrefab room, Position pos);
	void PlaceRoom(RoomPrefab room, Position pos);
	void PlaceDoor(Position pos);
	void MarkPossibleDoor(Position pos);

	bool isStarted = false;
	bool isFinished = false;

	std::vector<struct RoomPrefab> roomPrefabs;
	std::shared_ptr<RandomUtil> rand;
};

