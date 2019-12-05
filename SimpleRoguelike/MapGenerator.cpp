#include <iostream>

#include "MapGenerator.h"
#include "Map.h"
#include "RandomNumber.h"

int genArr[Map::MAP_WIDTH][Map::MAP_HEIGHT];
int mapW, mapH;

class RoomInfo {
public:
	int x1, x2, y1, y2;
	int id;
	bool carved = false;
	int Width() {
		return y2 - y1;
	}
	int Height() {
		return y2 - y1;
	}
};

void GenerateRoomsDungeon();
RoomInfo CarveRoom();



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
	GenerateRoomsDungeon();

	for (int y = 0; y < mapH; y++) {
		for (int x = 0; x < mapW; x++) {
			bool setWall = true;
			if (genArr[x][y] == 0) {
				setWall = false;
			}
			else if (genArr[x][y] == 1) {
				setWall = true;
			}
			map->GetCell(x, y)->SetupCell(setWall);
		}
	}

	map->SetAllKnown(true); //for debugging map gen
}

void GenerateRoomsDungeon() {
	for (int y = 0; y < mapH; y++) {
		for (int x = 0; x < mapW; x++) {
			genArr[x][y] = -1; //-1 means space has not been assigned yet
		}
	}

	int numOfRooms = 8;
	int roomsCarved = 0;
	int tries = 0;
	while (roomsCarved < numOfRooms && tries < 10) {
		RoomInfo rm = CarveRoom();
		if (rm.carved){
			roomsCarved++;
		}
		tries++;
	}

	for (int y = 0; y < mapH; y++) {
		for (int x = 0; x < mapW; x++) {
			if (genArr[x][y] == -1) {
				genArr[x][y] = 0;
			}
		}
	}

}
RoomInfo CarveRoom() {
	int attempts = 0;
	bool canCarve = false;
	int roomW, roomH, x1, x2, y1, y2;

	while (attempts < 10 && !canCarve) { //try 100 times to get a room to carve
		canCarve = true;
		roomW = RandomNumber::GetRandomInt(5, 10);
		roomH = RandomNumber::GetRandomInt(5, 10);
		x1 = RandomNumber::GetRandomInt(1, mapW - roomW - 1);
		y1 = RandomNumber::GetRandomInt(1, mapH - roomH - 1);
		x2 = x1 + roomW - 1;
		y2 = y1 + roomH - 1;
		for (int y = y1 - 1; y <= y2 + 1 && canCarve; y++) { //want to avoid placing rooms right against eachother
			for (int x = x1 - 1; x <= x2 + 1 && canCarve; x++) {
				if (genArr[x][y] != -1) {
					canCarve = false;
				}
			}
		}
		attempts++;
		
	}
	RoomInfo newRoom;
	if (canCarve) {
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
		
		newRoom.x1 = x1;
		newRoom.x2 = x2;
		newRoom.y1 = y1;
		newRoom.y2 = y2;
		newRoom.carved = true;
		return newRoom;
	}
	else {
		return newRoom;
	}
	
}