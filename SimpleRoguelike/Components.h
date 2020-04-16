#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include <unordered_map>
const int MAX_MAP_SIZE = 100;

struct Position {
	int x = 0;
	int y = 0;

	bool operator==(const Position& other) const{
		return (x == other.x && y == other.y);
	}

	bool operator<(const Position& other) const {
		return (x < other.x && y < other.y);
	}

	Position& operator+(int a){
		x += a;
		y += a;
		return *this;
	}

	Position& operator-(int a) {
		x -= a;
		y -= a;
		return *this;
	}
};

namespace std {

	template <>
	struct hash<Position>
	{
		std::size_t operator()(const Position& pos) const
		{
			using std::size_t;

			return (pos.x * 1000) + pos.y;
		}
	};

} 

struct Actor {
	int actionDebt = 0;
};

struct Renderable {
	int sprite = 0;
	float x = 0;
	float y = 0;
};

struct Info {
	std::string name = "Untitled";
	std::string info = "No description.";
};

struct PlayerControlled {

};

struct Active {

};

struct Map { //hardcoded default map
	int width = 6;
	int height = 6;

	bool cells[6][6] = {
		{true, true, true, true, true, true},
		{true, true, false, false, false, true},
		{true, false, false, false, false, true},
		{true, false, false, true, false, true},
		{true, false, false, false, false, true},
		{true, true, true, true, true, true}
	};

	int wallSprite = 3;
	int floorSprite = 1;

	//std::shared_ptr<bool[MAX_MAP_SIZE][MAX_MAP_SIZE]> visited;
	//std::shared_ptr<bool[MAX_MAP_SIZE][MAX_MAP_SIZE]> visible;

	std::unordered_map<Position, Entity> positionEntityMap;
};