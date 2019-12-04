#include <iostream>

#include "MapGenerator.h"
#include "Map.h"
#include "RandomNumber.h"

int genArr[Map::MAP_WIDTH][Map::MAP_HEIGHT];
int mapW, mapH;

void GenerateRoomsDungeon();
bool CarveRoom();

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
	while (roomsCarved < numOfRooms && tries < 99) {
		if (CarveRoom()) {
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
bool CarveRoom() {
	int roomW = RandomNumber::GetRandomInt(5, 10);
	int roomH = RandomNumber::GetRandomInt(5, 10);
	int x1 = RandomNumber::GetRandomInt(1, mapW - roomW - 1);
	int y1 = RandomNumber::GetRandomInt(1, mapH - roomH - 1);
	int x2 = x1 + roomW - 1;
	int y2 = y1 + roomH - 1;

	for (int y = y1-1; y <= y2+1; y++) { //wan't to avoid placing rooms right against eachother
		for (int x = x1-1; x <= x2+1; x++) {
			if (genArr[x][y] != -1) {
				return false; //cannot carve this room
			}
		}
	}
	for (int y = y1; y <= y2; y++) {
		for (int x = x1; x <= x2; x++) {
			if (x == x1 || y == y1 || x == x2 || y == y2){
				genArr[x][y] = 1;
			}else{
				genArr[x][y] = 0;
			}
		}
	}
	return true;
}