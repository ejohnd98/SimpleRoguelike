#pragma once
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <string>
#include <unordered_map>

#include "Constants.h"
#include "Animation.h"

enum class EventType {
	NULL_EVENT,
	ATTACK
};

enum class EaseType {
	LINEAR,
	SIN,
	BOUNCE_OUT,
	QUAD_OUT
};

enum class AIState {
	IDLE,
	WANDERING,
	ATTACKING,
	FLEEING
};

enum class InteractType {
	WAIT,
	MOVE,
	ATTACK,
	OPEN,
	CLOSE
};

enum class LayoutInfo {
	EMPTY,
	WALL,
	FLOOR,
	DOOR,
	POSSIBLE_DOOR,
	POSSIBLE_WALL,
	ENTRANCE,
	EXIT
};

struct FloatPosition {
	float x = 0;
	float y = 0;

	bool operator==(const FloatPosition& other) const {
		return (x == other.x && y == other.y);
	}

	bool operator<(const FloatPosition& other) const {
		return (x < other.x && y < other.y);
	}

	FloatPosition& operator+(const FloatPosition& other) {
		FloatPosition pos;
		pos.x = x + other.x;
		pos.y = y + other.y;
		return pos;
	}

	FloatPosition& operator-(const FloatPosition& other) {
		FloatPosition pos;
		pos.x = x - other.x;
		pos.y = y - other.y;
		return pos;
	}

	FloatPosition& operator+(const float a) {
		FloatPosition pos;
		pos.x = x + a;
		pos.y = y + a;
		return pos;
	}

	FloatPosition& operator-(const float a) {
		FloatPosition pos;
		pos.x = x - a;
		pos.y = y - a;
		return pos;
	}

	FloatPosition& operator*(const FloatPosition& other) {
		FloatPosition pos;
		pos.x = x * other.x;
		pos.y = y * other.y;
		return pos;
	}

	FloatPosition& operator*(const float a) {
		FloatPosition pos;
		pos.x = x * a;
		pos.y = y * a;
		return pos;
	}
};

struct Position {
	int x = 0;
	int y = 0;

	Position() {
		x = 0;
		y = 0;
	}

	Position(int a, int b) {
		x = a;
		y = b;
	}

	Position(FloatPosition pos) {
		x = (int)pos.x;
		y = (int)pos.y;
	}

	bool operator==(const Position& other) const{
		return (x == other.x && y == other.y);
	}

	bool operator<(const Position& other) const {
		return (x < other.x && y < other.y);
	}

	Position& operator+(const Position& other) {
		Position pos;
		pos.x = x + other.x;
		pos.y = y + other.y;
		return pos;
	}

	Position& operator-(const Position& other) {
		Position pos;
		pos.x = x - other.x;
		pos.y = y - other.y;
		return pos;
	}

	Position& operator+(const int a){
		Position pos;
		pos.x = x + a;
		pos.y = y + a;
		return pos;
	}

	Position& operator-(const int a) {
		Position pos;
		pos.x = x - a;
		pos.y = y - a;
		return pos;
	}

	Position& operator*(const int a) {
		Position pos;
		pos.x = x * a;
		pos.y = y * a;
		return pos;
	}

	void Normalize() {
		if (x != 0){ x = x / std::abs(x); }
		if (y != 0) { y = y / std::abs(y); }
	}

	float Magnitude() {
		return (float)sqrt((x * x) + (y * y));
	}

	Position Absolute() {
		Position abs;
		if (x != 0) { abs.x = x / std::abs(x); }
		if (y != 0) { abs.y = y / std::abs(y); }
		return abs;
	}

	std::vector<Position> Adjacents() {
		std::vector<Position> vec;
		vec.push_back({ x+1,y });
		vec.push_back({ x-1,y });
		vec.push_back({ x,y+1 });
		vec.push_back({ x,y-1 });
		return vec;
	}

	float Dist(const Position& other) {
		int ox = other.x - x;
		int oy = other.y - y;
		return (float)sqrt((ox * ox) + (oy * oy));
	}

	FloatPosition ToFloat() {
		return FloatPosition{ (float)x, (float)y };
	}

	std::string ToString() {
		std::string pos = "( ";
		pos += std::to_string(x);
		pos += ", ";
		pos += std::to_string(y);
		pos += " )";
		return pos;
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
	Sprite sprite = 0;
	Tileset tileset = MAIN_TILESET;
	FloatPosition position = {0,0};
};

struct AnimIdle {
	std::vector<Sprite> sprites;
	std::vector<int> frameLengths;

	int index = 0;
	int frameCounter = 0;

	void AnimStep() {
			frameCounter++;
			if (frameCounter > frameLengths[index]) {
				frameCounter = 0;
				index = (index+1)%sprites.size();
			}
	}
	Sprite CurrentSprite() {
		return sprites[index];
	}
};

struct AnimSprite {
	std::vector<Sprite> sprites;
	std::vector<int> frameLengths;
	bool loop;

	int index = 0;
	int frameCounter = 0;
	bool finished = false;

	void AnimStep() {
		if (!finished) {
			frameCounter++;
			if (frameCounter > frameLengths[index]) {
				frameCounter = 0;
				index++;
				if (index >= sprites.size()) {
					if (loop) {
						index = 0;
					}
					else {
						index--;
						finished = true;
					}
				}
			}
		}
	}
	Sprite CurrentSprite() {
		return sprites[index];
	}
};

struct AnimMove {
	FloatPosition start, end;
	float length;
	EaseType easeType = EaseType::QUAD_OUT;

	float time = 0;
	bool finished = false;

	void AnimStep() {
		if (!finished) { 
			time += 1/(length*60);
			if (time >= 1.0) {
				finished = true;
				time = 1.0;
			}
		}
	}
	FloatPosition CurrentPos() {
		if (finished) {
			return end;
		}
		else {
			switch (easeType) {
			case EaseType::LINEAR:
				return Animation::LinearEase(start, end, time);
			case EaseType::SIN:
				return Animation::SinEase(start, end, time);
			case EaseType::BOUNCE_OUT:
				return Animation::BounceEaseOut(start, end, time);
			case EaseType::QUAD_OUT:
				return Animation::QuadEaseOut(start, end, time);
			}
		}
	}
};

struct DeleteAfterAnim {};

struct Info {
	std::string name = "Untitled";
	std::string info = "No description.";
};

struct Stats {
	int hp = 10;
	int atk = 5;
	int def = 5;
	int mag = 5;
	int res = 5;
	int spd = 5;
	int sight = 8;
};

struct PlayerControlled {

};

struct AIControlled {
	AIState currentState = AIState::IDLE;
	Entity targetEntity = NULL_ENTITY;
	Position lastTargetPos;
};

struct ActiveAIEntity {

};

struct Active {

};

struct Openable {
	Sprite closedSprite = 0;
	Sprite openedSprite = 0;
	bool isOpen = false;
	bool locked = false;
};

struct Map { //hardcoded default map
	int width = 40;
	int height = 30;

	bool cells[MAX_MAP_SIZE][MAX_MAP_SIZE] = {
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,0,0,0,1,0,0,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,1},
		{1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,1,1,0,0,1,0,1,0,0,0,0,0,1,1,1,1,0,1,1,1,1},
		{1,0,0,0,1,0,0,1,1,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,1,1,0,0,1,1,1,0,0,0,1,0,1,1,1,1},
		{1,0,1,0,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,1},
		{1,0,0,0,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,0,1},
		{1,0,1,0,0,0,0,1,1,1,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,1,1,0,1},
		{1,0,1,1,1,1,1,1,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,1,0,1},
		{1,0,1,1,1,1,1,1,1,0,1,1,0,0,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,0,1},
		{1,0,0,0,0,0,1,1,1,0,1,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1},
		{1,1,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,1,0,1,1,1,0,0,0,0,0,0,0,0,1,1,0,1},
		{1,1,0,0,0,1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,0,1,1,0,0,0,0,1,0,0,0,0,0,0,1,1,0,0,1},
		{1,1,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,1,0,0,0,1},
		{1,1,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
		{1,1,0,0,0,0,0,0,0,1,1,1,0,0,1,1,1,1,1,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,1,1,0,0,0,1},
		{1,1,0,0,0,0,0,0,0,1,1,1,0,0,1,0,0,0,1,0,1,0,0,1,0,0,0,0,1,0,0,0,0,0,0,1,1,0,0,1},
		{1,1,0,0,0,0,0,0,0,1,1,1,0,0,1,0,0,0,1,0,1,1,1,1,0,1,1,1,0,0,0,0,0,0,0,0,1,1,0,1},
		{1,1,1,1,1,1,0,1,1,1,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1},
		{1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1},
		{1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1},
		{1,1,1,0,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,0,1,1,1,0,0,0,0,0,0,0,0,0,1,1,0,1},
		{1,1,1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
		{1,1,1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1},
		{1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,1,1,0,1,0,1,1,1,1,0,0,1,0,1},
		{1,1,1,0,0,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,0,0,1,0,0,0,0,1},
		{1,1,1,0,0,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,0,0,0,0,0,1,1,1},
		{1,1,1,0,0,1,1,1,1,1,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,1,0,0,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	};

	Sprite wallSprite = 3;
	Sprite floorSprite = 1;

	std::unordered_map<Position, Entity> positionEntityMap;

	bool GetCell(int x, int y) {
		return cells[y][x];
	}
};

struct TextLog {
	std::string text;
};

struct EventInfo {
	EventType type = EventType::NULL_EVENT;
	Entity source = NULL_ENTITY;
	Entity target = NULL_ENTITY;

	int damageDealt = -1;
	bool killed = false;
};

struct PathInfo {
	Position nextPos;
	int length = 99999;
	bool actorsBlocking = false;
};

enum class RoomType {
	REGULAR,
	ENTRANCE,
	EXIT
};

struct RoomPrefab {
	RoomType type = RoomType::REGULAR;
	LayoutInfo cells[MAX_PREFAB_SIZE][MAX_PREFAB_SIZE];
	int width = 0;
	int height = 0;
	std::vector<Position> possibleDoorPositions;
	std::vector<Position> doorPositions;
	std::vector<Position> entrances;
	std::vector<Position> exits;
};