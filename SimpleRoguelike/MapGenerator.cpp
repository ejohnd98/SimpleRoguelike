#include "MapGenerator.h"
#include "ECS.h"
#include "JSONHandler.h"
#include "EntityFactory.h"

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr<EntityFactory> entityFactory;
extern std::shared_ptr<MapSystem> mapSystem;

const int stepLengthMod = 1; //time to show a step
const int generationTimeMod = 2; //how many steps more to do
const int displayResultLength = 150 * stepLengthMod;
const int roomGenBaseLength = 100 * generationTimeMod * stepLengthMod;
const int tunnelGenBaseLength = 400 * generationTimeMod * stepLengthMod;

int roomGenLength = roomGenBaseLength;
int tunnelGenLength = tunnelGenBaseLength;
float roomDensity = 1.0f;
float tunnelAmount = 1.0f;

const int speedUp = 1;

MapGenerator::MapGenerator(int seed) {
	mapSeed = seed;
	auto prefabs = JSONHandler::ReadRoomJSONs("room_jsons");
	for (auto& rm : prefabs) {
		if (rm.type == RoomType::ENTRANCE) {
			printf("adding entrance\n");
			entrancePrefabs.push_back(rm);
		}
		else if (rm.type == RoomType::EXIT) {
			printf("adding exit\n");
			exitPrefabs.push_back(rm);
		}
		else {
			roomPrefabs.push_back(rm);
		}
	}

	assert(!entrancePrefabs.empty());
	assert(!exitPrefabs.empty());
	assert(!roomPrefabs.empty());

	rand = std::make_shared<RandomUtil>(mapSeed);
}

void MapGenerator::Reset(bool clearMap) {
	isStarted = false;
	isFinished = false;

	roomsPlaced = 0;
	counter = 0;
	possibleDoorPositions.clear();
	tunnelers.clear();
	ChangeState(MapGenState::INIT);

	if (clearMap) {
		mapSystem->Clear();
	}
	std::shared_ptr<Map> nextMap = std::make_shared<Map>();
	mapSystem->SetMap(nextMap);
	Begin(nextMap, genWidth, genHeight);
}

void MapGenerator::Begin(std::shared_ptr<Map> mapData, int w, int h, float tunnel, float density) {
	tunnelAmount = tunnel;
	roomDensity = density;
	map = mapData;
	map->width = w;
	map->height = h;
	genWidth = w;
	genHeight = h;
	mapSystem->SetMap(mapData);

	double lengthMod = 0.15f + 0.85f*((double)0.000125 * (double)w * (double)h);
	roomGenLength = (int)(roomGenBaseLength * lengthMod * roomDensity);
	tunnelGenLength = (int)(tunnelGenBaseLength * lengthMod * tunnelAmount);

	//clear space for new map
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			mapLayout[y][x] = LayoutInfo::EMPTY;
			debugGraphics[y][x] = 0;
		}
	}
	isStarted = true;
	isFinished = false;
}

void MapGenerator::GenerationStep() {
	int speedUpCounter = 0;

	while (speedUpCounter < speedUp && mapGenState != MapGenState::FINISHED) {
		speedUpCounter++;
		bool madeProgress = false;
		int timeOut = 0;
		counter++;
		while (!madeProgress && timeOut < 1000) {
			timeOut++;
			switch (mapGenState) {
			case MapGenState::INIT: {
				madeProgress = InitStep();
				break;
			}
			case MapGenState::INITIAL_ROOMS: {
				madeProgress = PlaceRoomsStep();
				break;
			}
			case MapGenState::TUNNELING: {
				madeProgress = TunnelStep();
				break;
			}
			case MapGenState::CLEANING: {
				madeProgress = CleanStep();
				break;
			}
			case MapGenState::POPULATING: {
				madeProgress = PopulateStep();
				break;
			}
			case MapGenState::DISPLAYING: {
				madeProgress = true;
				if (counter >= displayResultLength) {
					FinishMap();
				}
				break;
			}
			default:
				break;
			}
		}
	}
}

void MapGenerator::ChangeState(MapGenState newState) {
	onStateEnter = true;
	counter = 0;
	mapGenState = newState;
}

bool MapGenerator::InitStep() {
	bool madeProgress = false;

	int i = rand->GetRandomInt(0, 3);
	if (i == 0) { //start top left
		PlaceRandomRoom({ 0,0 }, { (int)(map->width * 0.4f), (int)(map->height * 0.4f) }, RoomType::ENTRANCE, false);
		PlaceRandomRoom({ (int)(map->width * 0.6f),(int)(map->height * 0.6f) }, { map->width, map->height }, RoomType::EXIT, false);
	}
	if (i == 1) { //start bottom right
		PlaceRandomRoom({ (int)(map->width * 0.6f),(int)(map->height * 0.6f) }, { map->width, map->height }, RoomType::ENTRANCE, false);
		PlaceRandomRoom({ 0,0 }, { (int)(map->width * 0.4f), (int)(map->height * 0.4f) }, RoomType::EXIT, false);
	}
	if (i == 2) { //start top right
		PlaceRandomRoom({ (int)(map->width * 0.6f), 0 }, { map->width, (int)(map->height * 0.4f) }, RoomType::ENTRANCE, false);
		PlaceRandomRoom({ 0,(int)(map->height * 0.6f) }, { (int)(map->width * 0.4f),map->height }, RoomType::EXIT, false);
	}
	if (i == 3) { //start bottom left
		PlaceRandomRoom({ 0,(int)(map->height * 0.6f) }, { (int)(map->width * 0.4f),map->height }, RoomType::ENTRANCE, false);
		PlaceRandomRoom({ (int)(map->width * 0.6f), 0 }, { map->width, (int)(map->height * 0.4f) }, RoomType::EXIT, false);
	}

	madeProgress = true;
	ChangeState(MapGenState::INITIAL_ROOMS);

	return madeProgress;
}

bool MapGenerator::PlaceRoomsStep() {
	bool madeProgress = false;

	if (counter % (3 * stepLengthMod) == 0) {
		madeProgress = PlaceRandomRoom({ 0,0 }, { map->width, map->height });
	}
	else {
		madeProgress = true;
	}

	if (counter >= roomGenLength) {
		ChangeState(MapGenState::TUNNELING);
	}

	return madeProgress;
}
bool MapGenerator::TunnelStep() {
	bool madeProgress = false;

	if (onStateEnter) {
		onStateEnter = false;
		FillBFS(5);
		HandlePossibleDoors();
		tunnelers.emplace_back(Tunneler{ {0, 0},{map->width - 3, map->height - 3}, {0, 1} });
		tunnelers.emplace_back(Tunneler{ {map->width - 3,  0},{0,map->height - 3}, {-1, 0} });
	}

	if (tunnelers.empty()) {
		FillBFS(2);
		HandlePossibleDoors();
		Position start = possibleDoorPositions[rand->GetRandomInt(0, possibleDoorPositions.size() - 1)];
		Position end = possibleDoorPositions[rand->GetRandomInt(0, possibleDoorPositions.size() - 1)];
		int counter2 = 0;
		while (start.Dist(end) < 20 && start.Dist(end) > 3 && counter2++ < 2000) {
			start = possibleDoorPositions[rand->GetRandomInt(0, possibleDoorPositions.size() - 1)];
			end = possibleDoorPositions[rand->GetRandomInt(0, possibleDoorPositions.size() - 1)];
		}
		tunnelers.emplace_back(Tunneler{ start, end, {0, 1} });

	}
	for (int i = 0; i < tunnelers.size(); i++) {
		UpdateTunneler(tunnelers[i]);
		if (tunnelers[i].currentPos == tunnelers[i].targetPos) {
			tunnelers.erase(tunnelers.begin() + i);
			i--;
		}
		madeProgress = true;
	}

	if (counter >= tunnelGenLength) {
		ChangeState(MapGenState::CLEANING);
	}

	return madeProgress;
}
bool MapGenerator::CleanStep() {
	bool madeProgress = false;

	madeProgress = true;
	if (counter % 3 == 0) {
		if (!RemoveDeadEnds()) {
			counter = 0;
			ProcessFinishedMap();
			std::cout << "Rooms placed: " << roomsPlaced << "\n";
			ChangeState(MapGenState::POPULATING);
		}
	}
	return madeProgress;
}
bool MapGenerator::PopulateStep() {
	bool madeProgress = true;

	FillBfsFromPoint(map->entrance);

	Position pos;
	int max = 0;
	for (int y = 0; y < map->height; y++) {
		for (int x = 0; x < map->width; x++) {
			int curr = bfs[y][x];
			if (curr > max) {
				max = curr;
				pos = { x,y };
			}
		}
	}

	for (int y = 0; y < map->height; y++) {
		for (int x = 0; x < map->width; x++) {
			int curr = bfs[y][x];
			bool placeEnemy = rand->GetRandomBool(0.01f + 0.02f * (float)curr / (float)max);
			if (placeEnemy && !mapSystem->BlocksMovement({ x,y })) {
				mapSystem->PlaceEntity(entityFactory->CreateActor(actorTypes[rand->GetRandomBool(0.8f) ? 2 : 1]), { x,y });
				debugGraphics[y][x] = 7;
			}
		}
	}

	ChangeState(MapGenState::DISPLAYING);

	return madeProgress;
}

void MapGenerator::FinishMap() {
	isFinished = true;
	isStarted = false;
	ChangeState(MapGenState::FINISHED);

	Position exit = map->exit;
	if (bfs[exit.y][exit.x] == -1 || DEBUG_MAP_GEN) {
		Reset(true);
	}
}

bool MapGenerator::IsStarted() {
	return isStarted;
}

bool MapGenerator::IsFinished() {
	return isFinished;
}

bool MapGenerator::PlaceRandomRoom(Position tl, Position br, RoomType type, bool connectToExisting) {
	int index = 0;
	RoomPrefab& room = roomPrefabs[0];

	if (type == RoomType::ENTRANCE) {
		index = rand->GetRandomInt(0, entrancePrefabs.size() - 1);
		room = entrancePrefabs[index];
	}else if (type == RoomType::EXIT) {
		index = rand->GetRandomInt(0, exitPrefabs.size() - 1);
		room = exitPrefabs[index];
	}else{
		index = rand->GetRandomInt(0, roomPrefabs.size() - 1);
		room = roomPrefabs[index];
	}
	

	if (roomsPlaced == 0 || !connectToExisting) {
		Position randPos = { rand->GetRandomInt(tl.x, br.x - room.width), rand->GetRandomInt(tl.y, br.y - room.height) };
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
			if (!DEBUG_MAP_GEN) {
				if (tile == LayoutInfo::DOOR) {
					mapSystem->PlaceEntity(entityFactory->CreateProp(propTypes[0]), { x,y });
				}
				if (tile == LayoutInfo::ENTRANCE) {
					mapSystem->PlaceEntity(entityFactory->CreateProp(propTypes[2]), { x,y });
					map->entrance = {x,y};
				}
				if (tile == LayoutInfo::EXIT) {
					mapSystem->PlaceEntity(entityFactory->CreateProp(propTypes[1]), { x,y });
					map->exit = { x,y };
				}
			}
		}
	}
}

bool MapGenerator::RoomFits(RoomPrefab room, Position pos) {
	for (int y = 0; y < room.height; y++) {
		int mapY = pos.y + y;
		for (int x = 0; x < room.width; x++) {
			int mapX = pos.x + x;
			if (!mapSystem->ValidPosition({ mapX, mapY })) {
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
		if (mapLayout[pos.y][pos.x] == LayoutInfo::DOOR && adjacentFloors == 2 && (mapSystem->ValidPosition(pos + Position{1,1}) && mapSystem->ValidPosition(pos + Position{ -1,-1 }))
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
		if (!mapSystem->ValidPosition(nextPos)) {
			continue;
		}
		float newDist = nextPos.Dist(tunneler.targetPos);
		float distWeight = (dist - newDist) * 1.25; //weight positions closer to target higher
		float homingWeight = (100/(newDist*newDist)); //weight positions closer to target higher
		float changeDirWeight = (tunneler.direction + offset).Magnitude() / 2; //if direction unchanged, weight higher
		float randWeight = rand->GetRandomValue(0.0, 1.5f);
		float avoidWeight = bfs[nextPos.y][nextPos.x] *1.2f;
		float avoidOverlapWeight = (mapLayout[nextPos.y][nextPos.x] == LayoutInfo::EMPTY) ? 0.0 : -1.0f;
		float edgeWeight = 0;
		if (nextPos.x == map->width - 1 || nextPos.x == 0 || nextPos.y == map->height - 1 || nextPos.y == 0) {
			edgeWeight = -1000;
		}
		
		float totalWeight = distWeight + homingWeight + changeDirWeight +avoidOverlapWeight + avoidWeight + randWeight + edgeWeight;
		
		if (totalWeight >= highestWeight) {
			highestWeight = totalWeight;
			newDirection = offset;
		}
	}

	tunneler.direction = newDirection;
	auto nextPos = tunneler.NextPos();
	if (mapSystem->ValidPosition(nextPos)) {
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

void MapGenerator::FillBfsFromPoint(Position pos) {
	for (int y = 0; y < map->height; y++) {
		for (int x = 0; x < map->width; x++) {
			bfs[y][x] = 9999999;
		}
	}
	bfs[pos.y][pos.x] = 0;

	std::queue<Position> queue;
	queue.push(pos);
	Position curr = pos;
	Position next;
	int currCost = 0;
	while (!queue.empty()) {
		curr = queue.front();
		currCost = bfs[curr.y][curr.x];
		queue.pop();
		for (int i = 0; i < 4; i++) {
			int ox = 0, oy = 0;
			if (i == 0) { ox = -1; }
			if (i == 1) { ox = 1; }
			if (i == 2) { oy = -1; }
			if (i == 3) { oy = 1; }
			next = curr + Position{ ox, oy };
			if (next.x>= 0 && next.x < map->width && next.y >= 0 && next.y < map->height) {
				if (bfs[next.y][next.x] > currCost + 1 && !map->cells[next.y][next.x]) {
					queue.push(next);
					bfs[next.y][next.x] = currCost + 1;
				}
			}
		}
	}
	for (int y = 0; y < map->height; y++) {
		for (int x = 0; x < map->width; x++) {
			if (bfs[y][x] == 9999999) {
				bfs[y][x] = -1;
			}
		}
	}
}

bool MapGenerator::RemoveDeadEnds() {;
	bool modified = false;
	for (int y = 0; y < map->height; y++) {
		for (int x = 0; x < map->width; x++) {
			if (mapLayout[y][x] == LayoutInfo::FLOOR && AdjacentWalls({ x,y }) >= 3) {
				mapLayout[y][x] = LayoutInfo::EMPTY;
				modified = true;
			}
		}
	}
	return modified;
}

int MapGenerator::AdjacentWalls(Position pos) {
	int count = 0;
	auto adjacents = pos.Adjacents();
	for (Position adj : adjacents) {
		if (!mapSystem->ValidPosition(adj)
			|| (mapLayout[adj.y][adj.x] == LayoutInfo::WALL || mapLayout[adj.y][adj.x] == LayoutInfo::EMPTY || mapLayout[adj.y][adj.x] == LayoutInfo::POSSIBLE_WALL)) {
			count++;
		}
	}
	return count;
}