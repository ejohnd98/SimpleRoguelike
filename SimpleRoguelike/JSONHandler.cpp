#include <stdio.h>
#include <iostream>
#include <filesystem>
#include <string>
#include <sstream>
#include <fstream>

#include "JSONHandler.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

std::vector<RoomPrefab> JSONHandler::ReadRoomJSONs(const std::string directory) { //reads JSON at path into a rapidJSON document
	std::vector<RoomPrefab> prefabs;
	for (const auto& entry : std::filesystem::directory_iterator(directory)) {
		std::string path = entry.path().u8string();
		std::string fileName = entry.path().stem().u8string();

		std::ifstream file(path);
		std::stringstream buffer;
		buffer << file.rdbuf();

		Document doc;
		doc.Parse(buffer.str().c_str());
		
		RoomPrefab room = ReadRoomPrefab(doc);
		prefabs.push_back(room);
		room = Rotate90(room);
		prefabs.push_back(room);
		room = Rotate90(room);
		prefabs.push_back(room);
		room = Rotate90(room);
		prefabs.push_back(room);

		std::cout << "read " << fileName << " of size: "<< room.width << ", "<< room.height<< " with " << room.doorPositions.size() <<" doors\n";
	}
	return prefabs;
}

RoomPrefab JSONHandler::ReadRoomPrefab(Document& doc) { //reads in actors from the provided doc. Currently does nothing with the data
	Value& walls = doc["walls"];
	Value& floors = doc["floors"];
	Value& doors = doc["doors"];
	Value& possibleDoors = doc["possibleDoors"];
	Value& openWalls = doc["openWalls"];
	Value& entrances = doc["entrances"];
	Value& exits = doc["exits"];

	RoomPrefab room;

	room.width = doc["width"].GetInt();
	room.height = doc["height"].GetInt();

	for (SizeType y = 0; y < room.height; y++) {
		for (SizeType x = 0; x < room.width; x++) {
			room.cells[y][x] = LayoutInfo::EMPTY;
		}
	}

	for (SizeType i = 0; i < walls.Size(); i++) {
		Position pos = ValueToPosition(walls[i]);
		room.cells[pos.y][pos.x] = LayoutInfo::WALL;
	}

	for (SizeType i = 0; i < floors.Size(); i++) {
		Position pos = ValueToPosition(floors[i]);
		room.cells[pos.y][pos.x] = LayoutInfo::FLOOR;
	}

	for (SizeType i = 0; i < doors.Size(); i++) {
		Position pos = ValueToPosition(doors[i]);
		room.cells[pos.y][pos.x] = LayoutInfo::DOOR;
		room.doorPositions.push_back(pos);
	}

	for (SizeType i = 0; i < possibleDoors.Size(); i++) {
		Position pos = ValueToPosition(possibleDoors[i]);
		room.cells[pos.y][pos.x] = LayoutInfo::POSSIBLE_DOOR;
		room.possibleDoorPositions.push_back(pos);
	}

	for (SizeType i = 0; i < openWalls.Size(); i++) {
		Position pos = ValueToPosition(openWalls[i]);
		room.cells[pos.y][pos.x] = LayoutInfo::POSSIBLE_WALL;
	}

	for (SizeType i = 0; i < entrances.Size(); i++) {
		Position pos = ValueToPosition(entrances[i]);
		room.cells[pos.y][pos.x] = LayoutInfo::ENTRANCE;
		room.entrances.push_back(pos);
		room.type = RoomType::ENTRANCE;
	}

	for (SizeType i = 0; i < exits.Size(); i++) {
		Position pos = ValueToPosition(exits[i]);
		room.cells[pos.y][pos.x] = LayoutInfo::EXIT;
		room.type = RoomType::EXIT;
	}

	return room;
}

Position JSONHandler::ValueToPosition(Value& val) {
	return Position{ val[0].GetInt(), val[1].GetInt() };
}

RoomPrefab JSONHandler::Rotate90(RoomPrefab& room) {
	RoomPrefab newRoom;
	newRoom.width = room.height;
	newRoom.height = room.width;
	newRoom.type = room.type;

	for (int y = 0; y < room.height; y++) {
		for (int x = 0; x < room.width; x++) {
			newRoom.cells[x][newRoom.width - y - 1] = room.cells[y][x];
		}
	}
	for (Position pos : room.doorPositions) {
		newRoom.doorPositions.push_back({ newRoom.width - pos.y - 1, pos.x });
	}
	for (Position pos : room.possibleDoorPositions) {
		newRoom.possibleDoorPositions.push_back({ newRoom.width - pos.y - 1, pos.x });
	}
	for (Position pos : room.entrances) {
		newRoom.entrances.push_back({ newRoom.width - pos.y - 1, pos.x });
	}
	for (Position pos : room.exits) {
		newRoom.exits.push_back({ newRoom.width - pos.y - 1, pos.x });
	}
	return newRoom;
}