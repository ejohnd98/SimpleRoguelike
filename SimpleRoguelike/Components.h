#pragma once
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <string>
#include <unordered_map>

#include "Constants.h"
#include "Animation.h"

enum class EaseType {
	LINEAR,
	SMOOTH
};

enum class AIState {
	IDLE,
	WANDERING,
	ATTACKING,
	FLEEING
};

enum class ActionType {
	MOVE,
	ATTACK,
	WAIT,
	NONE
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

	float Magnitude() {
		return sqrt((x * x) + (y * y));
	}

	FloatPosition ToFloat() {
		return FloatPosition{ (float)x, (float)y };
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
	FloatPosition position = {0,0};
};

struct AnimIdle {
	Sprite sprites[32];
	int length;
	int framesPerSprite;

	int index = 0;
	int frameCounter = 0;

	void AnimStep() {
			frameCounter++;
			if (frameCounter >= framesPerSprite) {
				frameCounter = 0;
				index = (index+1)%length;
			}
	}
	Sprite CurrentSprite() {
		return sprites[index];
	}
};

struct AnimSprite {
	Sprite sprites[32];
	int length;
	int framesPerSprite;
	bool loop;

	int index = 0;
	int frameCounter = 0;
	bool finished = false;

	void AnimStep() {
		if (!finished) {
			frameCounter++;
			if (frameCounter >= framesPerSprite) {
				frameCounter = 0;
				index++;
				if (index >= length) {
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
	EaseType easeType = EaseType::SMOOTH;

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
			case EaseType::SMOOTH:
				return Animation::SmoothEase(start, end, time);
			}
		}
	}
};

struct Info {
	std::string name = "Untitled";
	std::string info = "No description.";
};

struct Stats {
	int health = 10;
	int strength = 5;
	int defense = 5;
	int dexterity = 5;
};

struct PlayerControlled {

};

struct AIControlled {
	AIState currentState = AIState::IDLE;
};

struct ActiveAIEntity {

};

struct Map { //hardcoded default map
	int width = 19;
	int height = 8;

	bool cells[MAX_MAP_SIZE][MAX_MAP_SIZE] = {
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,1},
		{1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,0,1,0,1},
		{1,0,0,0,0,0,0,1,0,0,0,1,0,0,1,0,1,0,1},
		{1,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1},
		{1,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	};

	Sprite wallSprite = 3;
	Sprite floorSprite = 1;

	//std::shared_ptr<bool[MAX_MAP_SIZE][MAX_MAP_SIZE]> visited;
	//std::shared_ptr<bool[MAX_MAP_SIZE][MAX_MAP_SIZE]> visible;

	std::unordered_map<Position, Entity> positionEntityMap;

	bool GetCell(int x, int y) {
		return cells[y][x];
	}
};