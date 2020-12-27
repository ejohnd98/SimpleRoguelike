#pragma once
#include "System.h"
#include "RandomUtil.h"

enum class DunGenState {
	INIT_MAP_GEN,
	MAP_GEN,
	DISPLAYING,
	FINISHED
};

class DungeonGenerator
{
public:
	DungeonGenerator(int seed);
	void Reset();
	void Begin(std::shared_ptr<Dungeon> dungeonData, int levels);
	void GenerationStep();
	void FinishDungeon();
	bool IsStarted();
	bool IsFinished();

	std::shared_ptr<Dungeon> dungeon;
	std::shared_ptr<class MapGenerator> mapGen;

private:
	void ChangeState(DunGenState newState);
	bool InitStep();
	bool MapGenStep();

	DunGenState dunGenState = DunGenState::INIT_MAP_GEN;
	std::shared_ptr<RandomUtil> rand;
	int dungeonSeed = 0;
	bool isStarted = false;
	bool isFinished = false;
	bool onStateEnter = true;
	int genLevels = 0;
	int currentLevel = 0;
	int genWidth = 0, genHeight = 0;
	int counter = 0;
};

