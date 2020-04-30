#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include <unordered_map>

#include "Constants.h"

enum class AnimType {
	IDLE,
	MOVE,
	ATTACK
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

	float time = 0;
	bool finished = false;

	void AnimStep() {
		if (!finished) {
			time += (length / 60);
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
			return (start * (1.0 - time)) + (end * (time));
		}
	}
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
		{true, false, false, false, false, true},
		{true, false, false, true, false, true},
		{true, false, true, true, false, true},
		{true, false, true, false, false, true},
		{true, true, true, true, true, true}
	};

	Sprite wallSprite = 3;
	Sprite floorSprite = 1;

	//std::shared_ptr<bool[MAX_MAP_SIZE][MAX_MAP_SIZE]> visited;
	//std::shared_ptr<bool[MAX_MAP_SIZE][MAX_MAP_SIZE]> visible;

	std::unordered_map<Position, Entity> positionEntityMap;
};