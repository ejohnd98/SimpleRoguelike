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

	if (counter % 1 == 0) {
		int index = rand->GetRandomInt(0, roomPrefabs.size() - 1);
		auto& room = roomPrefabs[index];

		if (roomsPlaced == 0) {
			Position randPos = { rand->GetRandomInt(0, map->width - room.width), rand->GetRandomInt(0, map->height - room.height) };
			if (RoomFits(room, randPos)) {
				PlaceRoom(room, randPos);
			}
		}
		else {
			bool placedRoom = false;
			auto iOrder = rand->GetRandomSequence(room.possibleDoorPositions.size());
			for (int i = 0; i < iOrder.size(); i++) {
				if (placedRoom) {
					break;
				}
				Position roomDoorPos = room.possibleDoorPositions[iOrder[i]];
				auto jOrder = rand->GetRandomSequence(possibleDoorPositions.size());
				for (int j = 0; j < jOrder.size(); j++) {
					Position possiblePos = possibleDoorPositions[jOrder[j]];
					Position placePos = possiblePos - roomDoorPos;

					if (RoomFits(room, placePos)) {
						PlaceRoom(room, placePos);

						//printf("Placed room %d at (%d,%d)\n", index, placePos.x, placePos.y);

						PlaceDoor(placePos + roomDoorPos);
						//PlaceDoor(possiblePos);
						placedRoom = true;
						break;
					}
				}
			}
		}
	}

	if (counter > 450) {
		std::cout << "Rooms placed: " << roomsPlaced << "\n";
		FinishMap();
	}
}

void MapGenerator::FinishMap() {
	//convert to Map struct
	//HandlePossibleDoors();
	
	for (int y = 0; y < map->height; y++) {
		for (int x = 0; x < map->width; x++) {
			LayoutInfo tile = mapLayout[y][x];
			if (tile == LayoutInfo::EMPTY || tile == LayoutInfo::WALL || tile == LayoutInfo::POSSIBLE_WALL || tile == LayoutInfo::POSSIBLE_DOOR) {
				map->cells[y][x] = true;
				mapLayout[y][x] = LayoutInfo::WALL;
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
			auto roomTile = room.cells[y][x];
			auto mapTile = mapLayout[mapY][mapX];
			if (roomTile == LayoutInfo::EMPTY
				|| roomTile == mapTile
				||	mapTile == LayoutInfo::EMPTY
				|| (roomTile == LayoutInfo::POSSIBLE_DOOR && mapTile == LayoutInfo::WALL)) {
			} else {
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
			if (layoutInfo == LayoutInfo::POSSIBLE_DOOR) {
				MarkPossibleDoor({ mapX,mapY });
			}
			if (layoutInfo == LayoutInfo::DOOR) {
				PlaceDoor({ mapX,mapY });
			}
		}
	}
	roomsPlaced++;
}

void MapGenerator::PlaceDoor(Position pos) {
	mapLayout[pos.y][pos.x] = LayoutInfo::DOOR;
	std::remove(possibleDoorPositions.begin(), possibleDoorPositions.end(), pos);
}

void MapGenerator::MarkPossibleDoor(Position pos) {
	if (std::count(possibleDoorPositions.begin(), possibleDoorPositions.end(), pos) == 0) {
		possibleDoorPositions.push_back(pos);
	}
	mapLayout[pos.y][pos.x] = LayoutInfo::POSSIBLE_DOOR;
}

void MapGenerator::HandlePossibleDoors() {
	
	for (Position pos : possibleDoorPositions) {
		int adjacentPossibleDoors = 0;
		int adjacentDoors = 0;
		int adjacentFloors = 0;
		for (int i = 0; i < 4; i++) {
			Position offset;
			if (i == 0) { offset = { -1, 0 }; }
			if (i == 1) { offset = { 0, -1 }; }
			if (i == 2) { offset = { 1, 0 }; }
			if (i == 3) { offset = { 0, 1 }; }

			auto tile = mapLayout[pos.y + offset.y][pos.x + offset.x];
			if (tile == LayoutInfo::DOOR) {
				adjacentDoors++;
			}
			if (tile == LayoutInfo::POSSIBLE_DOOR) {
				adjacentPossibleDoors++;
			}
			if (tile == LayoutInfo::FLOOR) {
				adjacentFloors++;
			}
		}

		if (adjacentDoors == 0 && adjacentFloors >= 2 && adjacentPossibleDoors == 0) {
			PlaceDoor(pos);
		}
	}
}