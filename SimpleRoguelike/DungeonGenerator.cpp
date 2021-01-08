#include "DungeonGenerator.h"

#include "MapGenerator.h"
#include "ECS.h"
#include "EntityFactory.h"

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr<EntityFactory> entityFactory;
extern std::shared_ptr<MapSystem> mapSystem;
extern std::shared_ptr<DungeonSystem> dungeonSystem;


const int stepLengthMod = 1; //time to show a step
const int generationTimeMod = 2; //how many steps more to do
const int displayResultLength = 150 * stepLengthMod;

const int speedUp = 64;

DungeonGenerator::DungeonGenerator(int seed) {
	dungeonSeed = seed;
	rand = std::make_shared<RandomUtil>(dungeonSeed);
}

void DungeonGenerator::Reset() {
	isStarted = false;
	isFinished = false;

	counter = 0;
	ChangeState(DunGenState::INIT_MAP_GEN);

	std::shared_ptr<Dungeon> nextDungeon = std::make_shared<Dungeon>();
	dungeonSystem->SetDungeon(nextDungeon);
	//Begin(nextDungeon, genWidth, genHeight);
}

void DungeonGenerator::Begin(std::shared_ptr<Dungeon> dungeonData, int levels) {
	genLevels = levels;
	dungeonData->floorCount = levels;
	currentLevel = 0;
	dungeon = dungeonData;
	isStarted = true;
	isFinished = false;
	//mapGen = std::make_shared<MapGenerator>(rand->GetRandomInt(0, 9000000));

	dunGenState = DunGenState::INIT_MAP_GEN;
}

void DungeonGenerator::GenerationStep() {
	int speedUpCounter = 0;

	while (speedUpCounter < speedUp && dunGenState != DunGenState::FINISHED) {
		speedUpCounter++;
		bool madeProgress = false;
		int timeOut = 0;
		counter++;
		while (!madeProgress && timeOut < 1000) {
			timeOut++;
			switch (dunGenState) {
			case DunGenState::INIT_MAP_GEN: {
				madeProgress = InitStep();
				break;
			}
			case DunGenState::MAP_GEN: {
				madeProgress = MapGenStep();
				break;
			}
			case DunGenState::DISPLAYING: {
				madeProgress = true;
				
				if (counter >= displayResultLength) {
					dungeon->floors.push_back(mapGen->map);
					printf("dungeon floors vector size: %d\n", dungeon->floors.size());
					currentLevel++;

					if (currentLevel < genLevels) {
						ChangeState(DunGenState::INIT_MAP_GEN);
					}
					else {
						FinishDungeon();
					}
					
				}
				break;
			}
			default:
				break;
			}
		}
	}
}

void DungeonGenerator::ChangeState(DunGenState newState) {
	onStateEnter = true;
	counter = 0;
	dunGenState = newState;
}

bool DungeonGenerator::InitStep() {
	bool madeProgress = false;

	std::shared_ptr<Map> map = std::make_shared<Map>();
	mapSystem->SetMap(map);

	mapGen = std::make_shared<MapGenerator>(rand->GetRandomInt(0, 9000000));
	//mapGen->Reset(false);
	mapGen->Begin(map, 40, 30);

	

	ChangeState(DunGenState::MAP_GEN);

	return madeProgress;
}

bool DungeonGenerator::MapGenStep() {
	bool madeProgress = true;

	if(mapGen->IsFinished()) {
		ChangeState(DunGenState::DISPLAYING);
	}else {
		mapGen->GenerationStep();
	}

	return madeProgress;
}

void DungeonGenerator::FinishDungeon() {
	isFinished = true;
	isStarted = false;
	mapSystem->SetMap(dungeon->floors[0]);
}

bool DungeonGenerator::IsStarted() {
	return isStarted;
}

bool DungeonGenerator::IsFinished() {
	return isFinished;
}