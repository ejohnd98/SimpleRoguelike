#pragma once
#include "System.h"
#include "RandomUtil.h"

struct Tunneler {
	Position currentPos;
	Position targetPos;
	Position direction;

	Position NextPos() {
		return currentPos + direction;
	}
};

class MapGenerator
{
public:
	MapGenerator(int seed);
	void Reset();
	void Begin(std::shared_ptr<Map> mapData, int w, int h);
	bool GenerationStep(bool incrementCounter = true);
	void FinishMap();
	bool IsStarted();
	bool IsFinished();

	std::shared_ptr<Map> map;
	LayoutInfo mapLayout[MAX_MAP_SIZE][MAX_MAP_SIZE];
	int bfs[MAX_MAP_SIZE][MAX_MAP_SIZE];
	std::vector<Position> possibleDoorPositions;

private:
	bool PlaceRandomRoom(bool connectToExisting = true);
	void ProcessFinishedMap();
	bool RoomFits(RoomPrefab room, Position pos);
	void PlaceRoom(RoomPrefab room, Position pos);
	void PlaceDoor(Position pos);
	void PlaceFloor(Position pos);
	void MarkPossibleDoor(Position pos);
	void HandlePossibleDoors();
	void UpdateTunneler(Tunneler& tunneler);
	void FillBFS(int maxValue);
	bool BFSPass(int maxValue);

	std::vector<Tunneler> tunnelers;
	std::shared_ptr<RandomUtil> rand;
	int mapSeed = 0;
	bool isStarted = false;
	bool isFinished = false;
	int roomsPlaced = 0;
	int counter = 0;
	std::vector<struct RoomPrefab> roomPrefabs;

	
};

