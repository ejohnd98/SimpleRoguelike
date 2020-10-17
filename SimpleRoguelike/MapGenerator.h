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

enum class MapGenState {
	INIT,
	INITIAL_ROOMS,
	TUNNELING,
	CLEANING,
	POPULATING,
	DISPLAYING,
	FINISHED
};

class MapGenerator
{
public:
	MapGenerator(int seed);
	void Reset();
	void Begin(std::shared_ptr<Map> mapData, int w, int h, float tunnel = 0.5f, float density = 3.0f);
	void GenerationStep();
	void FinishMap();
	bool IsStarted();
	bool IsFinished();

	std::shared_ptr<Map> map;
	LayoutInfo mapLayout[MAX_MAP_SIZE][MAX_MAP_SIZE];
	int debugGraphics[MAX_MAP_SIZE][MAX_MAP_SIZE];
	int bfs[MAX_MAP_SIZE][MAX_MAP_SIZE];
	std::vector<Position> possibleDoorPositions;

private:
	void ChangeState(MapGenState newState);
	bool InitStep();
	bool PlaceRoomsStep();
	bool TunnelStep();
	bool CleanStep();
	bool PopulateStep();
	bool PlaceRandomRoom(Position tl, Position br, RoomType type = RoomType::REGULAR, bool connectToExisting = true);
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
	void FillBfsFromPoint(Position pos);
	bool RemoveDeadEnds();
	int AdjacentWalls(Position pos);

	MapGenState mapGenState = MapGenState::INIT;
	std::vector<Tunneler> tunnelers;
	std::shared_ptr<RandomUtil> rand;
	int mapSeed = 0;
	bool isStarted = false;
	bool isFinished = false;
	bool onStateEnter = true;
	int genWidth = 0, genHeight = 0;
	int roomsPlaced = 0;
	int counter = 0;
	std::vector<struct RoomPrefab> roomPrefabs;
	std::vector<struct RoomPrefab> entrancePrefabs;
	std::vector<struct RoomPrefab> exitPrefabs;
	
};

