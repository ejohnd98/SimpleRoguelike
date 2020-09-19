#include "MapGenerator.h"
#include "ECS.h"
#include "JSONHandler.h"

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr<EntityFactory> entityFactory;
std::shared_ptr<MapSystem> mapHandler;

const int stepLengthMod = 3; //time to show a step
const int generationTimeMod = 2; //how many steps more to do
const int resultShowTime = 8;

const int roomGenLength = 200 * generationTimeMod * stepLengthMod;
const int tunnelGenLength = roomGenLength + (300 * generationTimeMod * stepLengthMod);
const int displayResultLength = tunnelGenLength + (100 * stepLengthMod);

std::vector<PropType> propTypes {
	{"Door", "a door", 18, 20}
};

MapGenerator::MapGenerator(int seed) {
	mapSeed = seed;
	roomPrefabs = JSONHandler::ReadRoomJSONs("room_jsons");
	assert(!roomPrefabs.empty());

	rand = std::make_shared<RandomUtil>(mapSeed);
	mapHandler = std::make_shared<MapSystem>();
}

void MapGenerator::Reset() {
	isStarted = false;
	isFinished = false;
	//rand = std::make_shared<RandomUtil>(mapSeed);

	roomsPlaced = 0;
	counter = 0;
	possibleDoorPositions.clear();
	tunnelers.clear();
}

void MapGenerator::Begin(std::shared_ptr<Map> mapData, int w, int h) {
	map = mapData;
	map->width = w;
	map->height = h;
	mapHandler->SetMap(mapData);
	printf("test");
	//clear space for new map
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			mapLayout[y][x] = LayoutInfo::EMPTY;
		}
	}
	isStarted = true;
	isFinished = false;

	PlaceRandomRoom(false);
	PlaceRandomRoom(false);
	PlaceRandomRoom(false);

}

bool MapGenerator::GenerationStep(bool incrementCounter) {
	if (incrementCounter) {
		counter++;
	}
	bool madeProgress = false;
	if (counter % (stepLengthMod * 7) == 0 && counter < roomGenLength) {
		madeProgress = PlaceRandomRoom();
	}
	if (counter == roomGenLength) {
		FillBFS(5);
		HandlePossibleDoors();
		tunnelers.emplace_back(Tunneler{ {0, 0},{map->width - 1, map->height - 1}, {0, 1} });
		tunnelers.emplace_back(Tunneler{ {map->width - 1,  0},{0,map->height - 1}, {-1, 0} });
		madeProgress = true;
	}
	if (counter % (stepLengthMod) == 0 && counter > roomGenLength && counter <= tunnelGenLength) {
		if (tunnelers.empty()) {
			FillBFS(2);
			HandlePossibleDoors();
			Position start = possibleDoorPositions[rand->GetRandomInt(0, possibleDoorPositions.size()-1)];
			Position end = possibleDoorPositions[rand->GetRandomInt(0, possibleDoorPositions.size() - 1)];
			int counter = 0;
			while (start.Dist(end) < 20 && start.Dist(end) > 3 && counter++ < 2000) {
				start = possibleDoorPositions[rand->GetRandomInt(0, possibleDoorPositions.size() - 1)];
				end = possibleDoorPositions[rand->GetRandomInt(0, possibleDoorPositions.size() - 1)];
			}
			tunnelers.emplace_back(Tunneler{ start, end, {0, 1}});

		}
		for (int i = 0; i < tunnelers.size(); i++) {
			UpdateTunneler(tunnelers[i]);
			if (tunnelers[i].currentPos == tunnelers[i].targetPos) {
				tunnelers.erase(tunnelers.begin() + i);
				i--;
			}
			madeProgress = true;
		}
	}

	if (counter > displayResultLength) {
		std::cout << "Rooms placed: " << roomsPlaced << "\n";
		FinishMap();
		return true;
	}else if (counter == tunnelGenLength) {
		ProcessFinishedMap();
		return true;
	}
	else {
		return madeProgress;
	}

	
}

void MapGenerator::FinishMap() {
	//ProcessFinishedMap();

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

bool MapGenerator::PlaceRandomRoom(bool connectToExisting) {
	int index = rand->GetRandomInt(0, roomPrefabs.size() - 1);
	auto& room = roomPrefabs[index];

	if (roomsPlaced == 0 || !connectToExisting) {
		Position randPos = { rand->GetRandomInt(0, map->width - room.width), rand->GetRandomInt(0, map->height - room.height) };
		if (RoomFits(room, randPos)) {
			PlaceRoom(room, randPos);
		}
	}
	else {
		auto iOrder = rand->GetRandomSequence(room.possibleDoorPositions.size());
		for (int i = 0; i < iOrder.size(); i++) {
			Position roomDoorPos = room.possibleDoorPositions[iOrder[i]];
			auto jOrder = rand->GetRandomSequence(possibleDoorPositions.size());
			for (int j = 0; j < jOrder.size(); j++) {
				Position possiblePos = possibleDoorPositions[jOrder[j]];
				Position placePos = possiblePos - roomDoorPos;

				if (RoomFits(room, placePos)) {
					PlaceRoom(room, placePos);
					PlaceDoor(placePos + roomDoorPos);
					return true;
				}
			}
		}
	}
}

void MapGenerator::ProcessFinishedMap() {
	//convert to Map struct
	HandlePossibleDoors();

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
			if (tile == LayoutInfo::DOOR && !DEBUG_MAP_GEN) {
				mapHandler->PlaceEntity(entityFactory->CreateDoor(propTypes[0]), { x,y });
			}
		}
	}
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

void MapGenerator::PlaceFloor(Position pos) {
	mapLayout[pos.y][pos.x] = LayoutInfo::FLOOR;
	std::remove(possibleDoorPositions.begin(), possibleDoorPositions.end(), pos);
}

void MapGenerator::MarkPossibleDoor(Position pos) {
	if (std::count(possibleDoorPositions.begin(), possibleDoorPositions.end(), pos) == 0) {
		possibleDoorPositions.push_back(pos);
	}
	mapLayout[pos.y][pos.x] = LayoutInfo::POSSIBLE_DOOR;
}

void MapGenerator::HandlePossibleDoors() {
	std::vector<Position> tempVec;
	for (Position pos : possibleDoorPositions) {
		tempVec.push_back(pos);
	}
	for (int y = 0; y < map->height; y++) {
		for (int x = 0; x < map->width; x++) {
			if (mapLayout[y][x] == LayoutInfo::DOOR) {
				tempVec.push_back({x,y});
			}
		}
	}

	for (Position pos : tempVec) {
		if (mapLayout[pos.y][pos.x] != LayoutInfo::POSSIBLE_DOOR && mapLayout[pos.y][pos.x] != LayoutInfo::DOOR) {
			continue;
		}

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
		if (mapLayout[pos.y][pos.x] == LayoutInfo::POSSIBLE_DOOR && adjacentDoors == 0 && adjacentFloors == 2){
			PlaceDoor(pos);
		}
		if (mapLayout[pos.y][pos.x] == LayoutInfo::DOOR && adjacentFloors >= 3) {
			PlaceFloor(pos);
		}
		if (mapLayout[pos.y][pos.x] == LayoutInfo::DOOR && adjacentFloors == 2 && (mapHandler->ValidPosition(pos + Position{1,1}) && mapHandler->ValidPosition(pos + Position{ -1,-1 }))
			&& (!(mapLayout[pos.y + 1][pos.x] == LayoutInfo::FLOOR && mapLayout[pos.y - 1][pos.x] == LayoutInfo::FLOOR)
				&& !(mapLayout[pos.y][pos.x + 1] == LayoutInfo::FLOOR && mapLayout[pos.y][pos.x - 1] == LayoutInfo::FLOOR))) {
			PlaceFloor(pos);
		}
		
	}
}

void MapGenerator::UpdateTunneler(Tunneler& tunneler) {
	Position newDirection = tunneler.direction;
	float highestWeight = -999;
	float dist = tunneler.currentPos.Dist(tunneler.targetPos);

	for (int i = 0; i < 4; i++) {
		Position offset;
		if (i == 0) { offset = { -1, 0 }; }
		if (i == 1) { offset = { 0, -1 }; }
		if (i == 2) { offset = { 1, 0 }; }
		if (i == 3) { offset = { 0, 1 }; }
		Position nextPos = tunneler.currentPos + offset;
		if (!mapHandler->ValidPosition(nextPos)) {
			continue;
		}
		float newDist = nextPos.Dist(tunneler.targetPos);
		float distWeight = (dist - newDist) * 1.25; //weight positions closer to target higher
		float homingWeight = (100/(newDist*newDist)); //weight positions closer to target higher
		float changeDirWeight = (tunneler.direction + offset).Magnitude() / 2; //if direction unchanged, weight higher
		float randWeight = rand->GetRandomValue(0.0, 1.5f);
		float avoidWeight = bfs[nextPos.y][nextPos.x] *1.2f;
		float avoidOverlapWeight = (mapLayout[nextPos.y][nextPos.x] == LayoutInfo::EMPTY) ? 0.0 : -1.0f;
		float totalWeight = distWeight + homingWeight + changeDirWeight +avoidOverlapWeight + avoidWeight + randWeight;
		if (totalWeight >= highestWeight) {
			highestWeight = totalWeight;
			newDirection = offset;
		}
	}

	tunneler.direction = newDirection;
	auto nextPos = tunneler.NextPos();
	if (mapHandler->ValidPosition(nextPos)) {
		if (mapLayout[nextPos.y][nextPos.x] == LayoutInfo::EMPTY || mapLayout[nextPos.y][nextPos.x] == LayoutInfo::WALL || mapLayout[nextPos.y][nextPos.x] == LayoutInfo::POSSIBLE_WALL) {
			PlaceFloor(nextPos);
		}
		
		tunneler.currentPos = nextPos;
	}
}

void MapGenerator::FillBFS(int maxValue) {
	for (int y = 0; y < map->height; y++) {
		for (int x = 0; x < map->width; x++) {
			if (mapLayout[y][x] != LayoutInfo::EMPTY) {
				bfs[y][x] = 0;
				if (mapLayout[y][x] != LayoutInfo::WALL) {
					bfs[y][x] = 1;
				}
			}
			else {
				bfs[y][x] = maxValue;
			}
		}
	}
	while (BFSPass(maxValue)) {};
}

bool MapGenerator::BFSPass(int maxValue) {
	bool madeChanges = false;
	for (int y = 0; y < map->height; y++) {
		for (int x = 0; x < map->width; x++) {
			int current = bfs[y][x];
			for (int i = 0; i < 4; i++) {
				int ox = 0, oy = 0;
				if (i == 0) { ox = -1; }
				if (i == 1) { ox = 1; }
				if (i == 2) { oy = -1; }
				if (i == 3) { oy = 1; }
				if (x + ox > 0 && x + ox < map->width && y + oy > 0 && y + oy < map->height) {
					if (current > bfs[y + oy][x + ox] + 1) {
						bfs[y][x] = std::min( bfs[y + oy][x + ox] + 1, maxValue);
						madeChanges = true;
						//printf("made changes\n");
					}
				}
			}
		}
	}
	return madeChanges;
}