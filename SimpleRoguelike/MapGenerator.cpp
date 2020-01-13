#include <iostream>
#include <vector>

#include "MapGenerator.h"
#include "Coordinate.h"
#include "Pathfinder.h"
#include "Prop.h"
#include "Map.h"
#include "RandomNumber.h"

/*
	This class is very rough at the moment, as the basic map generation is still being hammered out.
	Eventually the magic numbers like number of rooms and number of spawned actors will be passed in when creating a map
	But for now, I'm just trying to create something that feels alright, then I'll clean it up properly after that's done.
*/

class RoomInfo { //stores the basic data for a room once it's created, and has functions related to cells within a room
public:
	int x1, x2, y1, y2;
	int id;
	bool carved = false; //has this room been carved into the map yet
	bool containsEntrance = false;
	bool containsExit = false;
	int Width() {
		return x2 - x1 + 1;
	}
	int Height() {
		return y2 - y1 + 1;
	}
	Coordinate Center() {
		return Coordinate((x1 + x2) * 0.5, (y1 + y2) * 0.5);
	}
	Coordinate GetCellClosestTo(int xt, int yt) { //gets the room cell closest to the given point, excluding corners (created for connecting rooms)
		int x = x1 + 1;
		int y = y1 + 1;
		while (x < x2 - 1 && x < xt) {
			x++; //get coordinate in room closest to target (but not a corner)
		}
		while (y < y2 - 1 && y < yt) {
			y++; //get coordinate in room closest to target (but not a corner)
		}
		return Coordinate(x, y);
	}
	Coordinate GetRandomEmptyCell(Map* map) {
		int x = RandomNumber::GetRandomInt(x1+1, x2-1);
		int y = RandomNumber::GetRandomInt(y1+1, y2-1);

		while (map->GetCell(x, y)->MovementBlocked()) {
			x = RandomNumber::GetRandomInt(x1 + 1, x2 - 1);
			y = RandomNumber::GetRandomInt(y1 + 1, y2 - 1);
		}
		return Coordinate(x, y);
	}
	bool IsInRoom(int x, int y) {
		return (x >= x1 && x <= x2 && y >= y1 && y <= y2);
	}
};

int genArr[Map::MAP_WIDTH][Map::MAP_HEIGHT]; //used when creating map. -1 is unset, 0 is empty, and 1 is a wall
int mapW, mapH;
int numOfRooms = 0;
std::vector<RoomInfo> rooms;

void GenerateRoomsDungeon(int roomCount);
RoomInfo CarveRoom();
void ConnectRooms(RoomInfo rm1, RoomInfo rm2);
void FillIfNotSet(int x, int y, int set);
void ErodeMap(int birthLimit = 7, int deathLimit = 9);
int SurroundingWallCount(int xs, int ys);
int AdjacentWallCountMap(int xs, int ys, Map* map);
void PlaceStairways(int floor, int numOfFloors, Map* map);
void PlaceDoors(int floor, int numOfFloors, Map* map);
void PlaceActors(int floor, int numOfFloors, Map* map);
int GetRoomIndexFromPos(int x, int y);

Map* MapGenerator::GenerateNewMap(int w, int h) {
	mapW = w;
	mapH = h;
	Map* map = new Map(mapW, mapH);
	GenerateMap(map, w, h);
	return map;
}

void MapGenerator::GenerateMap(Map* map, int w, int h) {
	mapW = w;
	mapH = h;
	map->SetAllKnown(false);

	GenerateRoomsDungeon(5);

	for (int y = 0; y < mapH; y++) { //Sets up the Map's cells based on the generated genArr
		for (int x = 0; x < mapW; x++) {
			bool setWall;
			if (genArr[x][y] == 0) {
				setWall = false;
			}
			else if (genArr[x][y] == 1) {
				setWall = true;
			}
			else {
				setWall = true;
			}
			map->GetCell(x, y)->SetupCell(setWall);
		}
	}
}

void GenerateRoomsDungeon(int roomCount) {
	numOfRooms = 0;
	while (numOfRooms != roomCount) { //generate room layouts until 5 rooms are generated
		rooms.clear();
		numOfRooms = 0;
		for (int y = 0; y < mapH; y++) {
			for (int x = 0; x < mapW; x++) {
				genArr[x][y] = -1; //-1 indicates space has not been assigned yet
			}
		}
		
		int failedTries = 0;
		while (failedTries < 50 && numOfRooms < roomCount) { //carve rooms until room requirement has been met, or failed 50 times
			RoomInfo rm = CarveRoom();
			if (rm.carved) {
				rooms.push_back(rm);
				numOfRooms++;
			}
			else {
				failedTries++;
			}
		}
	}

	//connects rooms (hardcoded to 5 rooms currently)
	ConnectRooms(rooms.at(0), rooms.at(1));
	ConnectRooms(rooms.at(0), rooms.at(2));
	ConnectRooms(rooms.at(0), rooms.at(3));
	ConnectRooms(rooms.at(3), rooms.at(4));
	ConnectRooms(rooms.at(2), rooms.at(3));

	//make all unassigned cells into walls
	for (int y = 0; y < mapH; y++) { 
		for (int x = 0; x < mapW; x++) {
			if (genArr[x][y] == -1) {
				genArr[x][y] = 1;
			}
		}
	}

	//optionally erode map (unused at the moment)
	int erodeSteps = 0;
	for (int i = 0; i < erodeSteps; i++) { 
		ErodeMap();
	}
}
void ConnectRooms(RoomInfo rm1, RoomInfo rm2) {
	Coordinate center1 = rm1.Center();
	Coordinate center2 = rm2.Center();
	int dirX = center2.x - center1.x;
	int dirY = center2.y - center1.y;
	if (dirX != 0) {
		dirX = dirX / abs(dirX); //will be +1 if rm2 is right of rm1, -1 otherwise
	}
	if (dirY != 0) {
		dirY = dirY / abs(dirY); //will be +1 if rm2 is below rm1, -1 otherwise
	}

	Coordinate start = rm1.GetCellClosestTo(center2.x, center2.y);
	Coordinate end = rm2.GetCellClosestTo(start.x, start.y);
	genArr[start.x][start.y] = 0;
	genArr[end.x][end.y] = 0;
	while (!start.IsCoord(end)) {
		if (genArr[start.x + dirX][start.y] == -1 && start.x != end.x) {
			FillIfNotSet(start.x, start.y+1, 1);
			FillIfNotSet(start.x, start.y-1, 1);
			FillIfNotSet(start.x - dirX, start.y, 1);
			start.x += dirX;
			genArr[start.x][start.y] = 0;
		}
		else if (genArr[start.x][start.y + dirY] == -1 && start.y != end.y) {
			FillIfNotSet(start.x + 1, start.y, 1);
			FillIfNotSet(start.x - 1, start.y, 1);
			FillIfNotSet(start.x, start.y - dirY, 1);
			start.y += dirY;
			genArr[start.x][start.y] = 0;
		}else if (start.x != end.x) {
			FillIfNotSet(start.x, start.y + 1, 1);
			FillIfNotSet(start.x, start.y - 1, 1);
			FillIfNotSet(start.x - dirX, start.y, 1);
			start.x += dirX;
			genArr[start.x][start.y] = 0;
		}
		else if (start.y != end.y) {
			FillIfNotSet(start.x + 1, start.y, 1);
			FillIfNotSet(start.x - 1, start.y, 1);
			FillIfNotSet(start.x, start.y - dirY, 1);
			start.y += dirY;
			genArr[start.x][start.y] = 0;
		}
		else {
			std::cout << "ERROR in connecting rooms: (" << start.x << ", " << start.y << "), (" << end.x << ", " << end.y << ")\n";
		}
	}
}

void FillIfNotSet(int x, int y, int set) {
	if (x >= 0 && x < mapW && y >= 0 && y < mapH) {
		if (genArr[x][y] == -1) {
			genArr[x][y] = set;
		}
	}
}

RoomInfo CarveRoom() { //Attempt to carve a room
	RoomInfo newRoom;
	int roomW, roomH, x1, x2, y1, y2;
	int attempts = 0;
	bool canCarve = false;
	while (attempts < 1 && !canCarve) {
		roomW = RandomNumber::GetRandomInt(6, 13);
		roomH = RandomNumber::GetRandomInt(6, 13);
		x1 = RandomNumber::GetRandomInt(1, mapW - roomW - 1);
		y1 = RandomNumber::GetRandomInt(1, mapH - roomH - 1);
		x2 = x1 + roomW - 1;
		y2 = y1 + roomH - 1;
		//check if any cells within new room are already set
		canCarve = true;
		for (int y = y1 - 1; y <= y2 + 1 && canCarve; y++) { //cycle through room cells, but stop if it can't be carved
			for (int x = x1 - 1; x <= x2 + 1 && canCarve; x++) {
				if (genArr[x][y] != -1) { //encountered already set cell, so can't carve room
					canCarve = false;
				}
			}
		}
		attempts++;
	}
	if (canCarve) {
		//carve room into genArr
		for (int y = y1; y <= y2; y++) {
			for (int x = x1; x <= x2; x++) {
				if (x == x1 || y == y1 || x == x2 || y == y2) {
					genArr[x][y] = 1;
				}
				else {
					genArr[x][y] = 0;
				}
			}
		}
		//fill in room values
		newRoom.x1 = x1;
		newRoom.x2 = x2;
		newRoom.y1 = y1;
		newRoom.y2 = y2;
		newRoom.carved = true;
	}
	return newRoom;
	
}

void MapGenerator::PopulateDungeon(int floor, int numOfFloors, Map* map) { //places stairs between levels
	mapW = map->GetWidth();
	mapH = map->GetHeight();
	PlaceStairways(floor, numOfFloors, map);
	PlaceDoors(floor, numOfFloors, map);
	PlaceActors(floor, numOfFloors, map);
}

void ErodeMap(int birthLimit, int deathLimit) {
	bool newMap[Map::MAP_WIDTH][Map::MAP_HEIGHT];
	for (int y = 0; y < mapH; y++) {
		for (int x = 0; x < mapW; x++) {
			newMap[x][y] = (genArr[x][y] == 1);
		}
	}
	for (int y = 0; y < mapH; y++) {
		for (int x = 0; x < mapW; x++) {
			int cnt = SurroundingWallCount(x, y);
			if (genArr[x][y] == 1) {
				if (cnt < deathLimit) {
					newMap[x][y] = false;
				}
				else{
					newMap[x][y] = true;
				}
			}
			else {
				if (cnt > birthLimit) {
					newMap[x][y] = true;
				}
				else{
					newMap[x][y] = false;
				}
			}
		}
	}
	for (int y = 0; y < mapH; y++) {
		for (int x = 0; x < mapW; x++) {
			if (newMap[x][y]) {
				genArr[x][y] = 1;
			}
			else {
				genArr[x][y] = 0;
			}
		}
	}
}

int SurroundingWallCount(int xs, int ys) {
	int cnt = 0;
	for (int y = ys - 1; y < ys + 2; y++) {
		for (int x = xs - 1; x < xs + 2; x++) {
			if (x >= 0 && x < mapW && y >= 0 && y < mapH) {
				if (genArr[x][y] == 1) {
					cnt++;
				}
			}
			else {
				cnt++;
			}
		}
	}
	return cnt;
}
int AdjacentWallCountMap(int xs, int ys, Map* map) {
	int cnt = 0;
	if (map->IsWall(xs + 1, ys)) {
		cnt++;
	}
	if (map->IsWall(xs, ys + 1)) {
		cnt++;
	}
	if (map->IsWall(xs - 1, ys)) {
		cnt++;
	}
	if (map->IsWall(xs, ys - 1)) {
		cnt++;
	}
	return cnt;
}

void PlaceStairways(int floor, int numOfFloors, Map* map) {
	bool placeEntrance = (floor != 0);
	bool placeExit = (floor != numOfFloors - 1);

	//choose corner to place entrance in:
	bool startLeft = RandomNumber::GetRandomBool(0.5);
	bool startTop = RandomNumber::GetRandomBool(0.5);
	int entranceX, entranceY, xSearchDir, ySearchDir = 0;
	if (startLeft) {
		xSearchDir = 1;
		entranceX = 0;
	}
	else {
		xSearchDir = -1;
		entranceX = mapW - 1;
	}
	if (startTop) {
		ySearchDir = 1;
		entranceY = 0;
	}
	else {
		ySearchDir = -1;
		entranceY = mapH - 1;
	}

	int heatMap[Map::MAP_WIDTH][Map::MAP_HEIGHT];

	bool spotFound = false;
	for (int x = entranceX; (x < mapW && x >= 0) && !spotFound; x += xSearchDir) { //find first empty spot starting from top left of map
		for (int y = entranceY; (y < mapH && y >= 0) && !spotFound; y += ySearchDir) {
			if (!map->IsWall(x, y)) {
				entranceX = x;
				entranceY = y;
				spotFound = true;
			}
		}
	}

	Pathfinder::FillHeatMap(entranceX, entranceY, map, &heatMap);
	int exitX = 0, exitY = 0;
	int largestDist = 0;
	for (int x = 0; x < mapW; x++) { //find spot farthest from entrance using filled heatmap
		for (int y = 0; y < mapH; y++) {
			if (heatMap[x][y] > largestDist) {
				largestDist = heatMap[x][y];
				exitX = x;
				exitY = y;
			}
		}
	}

	if (placeEntrance) {
		Prop* entrance = new Prop("Ascending Stairs", 16, Command::PREV_MAP);
		map->PlaceProp(entrance, entranceX, entranceY);
		map->entrance = map->GetCell(entranceX, entranceY);
		int rmIndex = GetRoomIndexFromPos(entranceX, entranceY);
		if (rmIndex >= 0) {
			rooms[rmIndex].containsEntrance = true;
		}
	}
	else {
		Prop* entrance = new Prop("Dungeon Exit", 16, Command::NONE);
		map->PlaceProp(entrance, entranceX, entranceY);
		map->entrance = map->GetCell(entranceX, entranceY);
		int rmIndex = GetRoomIndexFromPos(entranceX, entranceY);
		if (rmIndex >= 0) {
			rooms[rmIndex].containsEntrance = true;
		}
	}
	if (placeExit) {
		Prop* exit = new Prop("Descending Stairs", 17, Command::NEXT_MAP);
		map->PlaceProp(exit, exitX, exitY);
		map->exit = map->GetCell(exitX, exitY);
		int rmIndex = GetRoomIndexFromPos(exitX, exitY);
		if (rmIndex >= 0) {
			rooms[rmIndex].containsExit = true;
		}
	}
	else {
		Prop* treasure = new Prop("Treasure", 86, Command::WON_THE_GAME);
		map->PlaceProp(treasure, exitX, exitY);
	}
}

void PlaceDoors(int floor, int numOfFloors, Map* map) {
	for (int i = 0; i < numOfRooms; i++) {
		
		int x1 = rooms[i].x1;
		int x2 = rooms[i].x2;
		int y1 = rooms[i].y1;
		int y2 = rooms[i].y2;
		for (int x = x1; x <= x2; x++) {
			for (int y = y1; y <= y2; y++) {
				if (x == x1 || x == x2 || y == y1 || y == y2) { //if on border of room
					if (!map->IsWall(x, y)) { //if cell is free place door
						if (AdjacentWallCountMap(x, y, map) == 2) {
							Prop* door = new Prop("Door", 18, Command::USE_DOOR,true,true);
							map->PlaceProp(door, x, y);
						}
					}
				}
			}
		}
		
	}
}

void PlaceActors(int floor, int numOfFloors, Map* map) {
	int actorsToPlace = RandomNumber::GetRandomInt(numOfRooms*0.75, numOfRooms*1.25); //determine how many actors to place in dungeon
	int actorsPlaced = 0;
	int currentRoom = 0;
	while (actorsPlaced < actorsToPlace) { //while actors still need placing, cycle through non-entrance rooms on current map and place actor
		if (!rooms[currentRoom].containsEntrance) {
			Coordinate c = rooms[currentRoom].GetRandomEmptyCell(map);
			//place actor at coordinate c (placeholder actor for now)
			Actor* skel = new Actor("Skeleton", 34);
			if (map->PlaceActor(skel, c.x, c.y)) {
				skel->SetFaction(0);
				std::cout << "Placed " << skel->GetName() << " succesfully" << "\n";
			}
			actorsPlaced++;
		}
		currentRoom = (currentRoom + 1) % numOfRooms;
	}
}

int GetRoomIndexFromPos(int x, int y) {
	for (int i = 0; i < rooms.size(); i++) {
		if (rooms[i].IsInRoom(x, y)) {
			return i;
		}
	}
	return -1;
}