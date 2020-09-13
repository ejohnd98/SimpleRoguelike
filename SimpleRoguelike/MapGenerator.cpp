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
	isStarted = true;
	isFinished = false;
}

int roomsPlaced = 0;

void MapGenerator::GenerationStep() {
	counter++;

	if (counter % 3 == 0) {
		int index = rand->GetRandomInt(0, roomPrefabs.size() - 1);
		auto& room = roomPrefabs[index];

		if (roomsPlaced == 0) {
			Position randPos = { rand->GetRandomInt(0, map->width - room.width), rand->GetRandomInt(0, map->height - room.height) };
			if (RoomFits(room, randPos)) {
				printf("Placed room randomly\n");
				PlaceRoom(room, randPos);
			}
		}
		else {
			bool placedRoom = false;
			for (int i = 0; i < room.doorPositions.size(); i++) {
				if (placedRoom) {
					break;
				}
				for (Position pos : possibleDoorPositions) {
					Position placePos = pos - room.doorPositions[i];

					if (RoomFits(room, placePos)) {
						PlaceRoom(room, placePos);
						placedRoom = true;
					}
					else if (RoomFits(room, placePos + Position{0,1})) {
						placePos = placePos + Position{ 0,1 };
						PlaceRoom(room, placePos);
						placedRoom = true;
					}
					else if (RoomFits(room, placePos - Position{ 0,1 })) {
						placePos = placePos - Position{ 0,1 };
						PlaceRoom(room, placePos);
						placedRoom = true;
					}
					else if (RoomFits(room, placePos + Position{ 1,0 })) {
						placePos = placePos + Position{ 1,0 };
						PlaceRoom(room, placePos);
						placedRoom = true;
					}
					else if (RoomFits(room, placePos - Position{ 1,0 })) {
						placePos = placePos - Position{ 1,0 };
						PlaceRoom(room, placePos);
						placedRoom = true;
					}
					if (placedRoom) {
						Position origPos = pos - room.doorPositions[i];
						printf("Placed room %d at (%d,%d), offset from (%d, %d)\n", index, placePos.x, placePos.y, origPos.x, origPos.y);
					
						PlaceDoor(placePos + room.doorPositions[i]);
						PlaceDoor(pos);
						break;
					}
				}
			}
		}
	}

	if (counter > 250) {
		std::cout << "Rooms placed: " << roomsPlaced << "\n";
		FinishMap();
	}
}

void MapGenerator::FinishMap() {
	//convert to Map struct
	
	for (int y = 0; y < map->height; y++) {
		for (int x = 0; x < map->width; x++) {
			LayoutInfo tile = mapLayout[y][x];
			if (tile == LayoutInfo::EMPTY || tile == LayoutInfo::WALL || tile == LayoutInfo::POSSIBLE_WALL || tile == LayoutInfo::POSSIBLE_DOOR) {
				map->cells[y][x] = true;
			}
			else {
				map->cells[y][x] = false;
			}
			if (tile == LayoutInfo::DOOR) {
				mapHandler->PlaceEntity(entityFactory->CreateDoor(propTypes[0]), {x,y});
			}
		}
	}

	mapHandler->map = nullptr;
	isFinished = true;
	isStarted = false;
}

bool MapGenerator::IsStarted() {
	return isStarted;
}

bool MapGenerator::IsFinished() {
	return isFinished;
}

bool MapGenerator::RoomFits(RoomPrefab room, Position pos) {
	for (int y = 0; y < room.height; y++) {
		int mapY = pos.y + y;
		for (int x = 0; x < room.width; x++) {
			int mapX = pos.x + x;
			if (!mapHandler->ValidPosition({ mapX, mapY })) {
				return false;
			}
			if (room.cells[y][x] != LayoutInfo::EMPTY && mapLayout[mapY][mapX] != LayoutInfo::EMPTY) {
				return false;
			}
		}
	}
}

void MapGenerator::PlaceRoom(RoomPrefab room, Position pos) {
	for (int y = 0; y < room.height; y++) {
		int mapY = pos.y + y;
		for (int x = 0; x < room.width; x++) {
			int mapX = pos.x + x;
			auto layoutInfo = room.cells[y][x];
			if (layoutInfo != LayoutInfo::EMPTY) {
				mapLayout[mapY][mapX] = room.cells[y][x];
			}
			if (layoutInfo == LayoutInfo::DOOR || layoutInfo == LayoutInfo::POSSIBLE_DOOR) {
				MarkPossibleDoor({ mapX,mapY });
			}
		}
	}
	roomsPlaced++;
}

void MapGenerator::PlaceDoor(Position pos) {
	if (std::count(doorPositions.begin(), doorPositions.end(), pos) == 0) {
		doorPositions.push_back(pos);
	}
	mapLayout[pos.y][pos.x] = LayoutInfo::DOOR;
	std::remove(possibleDoorPositions.begin(), possibleDoorPositions.end(), pos);
}

void MapGenerator::MarkPossibleDoor(Position pos) {
	if (std::count(possibleDoorPositions.begin(), possibleDoorPositions.end(), pos) == 0) {
		possibleDoorPositions.push_back(pos);
	}
	mapLayout[pos.y][pos.x] = LayoutInfo::POSSIBLE_DOOR;
}