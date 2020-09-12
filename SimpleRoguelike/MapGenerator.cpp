#include "MapGenerator.h"
#include "ECS.h"
#include "JSONHandler.h"

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr<EntityFactory> entityFactory;
std::shared_ptr<MapSystem> mapHandler;
int counter = 0;

std::vector<PropType> propTypes {
	{"Door", "a door", 18, 20}
};

MapGenerator::MapGenerator(int seed) {
	roomPrefabs = JSONHandler::ReadRoomJSONs("room_jsons");
	assert(!roomPrefabs.empty());

	rand = std::make_shared<RandomUtil>(seed);
	mapHandler = std::make_shared<MapSystem>();

	for (int i = 0; i < 10; i++) {
		std::cout << rand->GetRandomInt(0, 100) << ", ";
	}
	std::cout << "\n";
}

void MapGenerator::Begin(std::shared_ptr<Map> mapData, int w, int h) {
	map = mapData;
	map->width = w;
	map->height = h;
	mapHandler->SetMap(mapData);

	//clear space for new map
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			mapLayout[y][x] = LayoutInfo::EMPTY;
		}
	}

}

void MapGenerator::GenerationStep() {
	counter++;
	if (counter > 300) {
		FinishMap();
	}
}

void MapGenerator::FinishMap() {
	//convert to Map struct
	
	for (int y = 0; y < map->height; y++) {
		for (int x = 0; x < map->width; x++) {
			LayoutInfo tile = mapLayout[y][x];
			if (tile == LayoutInfo::EMPTY || tile == LayoutInfo::WALL || tile == LayoutInfo::POSSIBLE_WALL) {
				//map->cells[y][x] = true;
			}
			else {
				//map->cells[y][x] = false;
			}
		}
	}
	for (auto& pos : doorPositions) {
		mapHandler->PlaceEntity(entityFactory->CreateDoor(propTypes[0]), pos);
	}

	mapHandler->map = nullptr;
	isFinished = true;
}

bool MapGenerator::IsFinished() {
	return isFinished;
}