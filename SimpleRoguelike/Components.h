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

struct Position {
	int x = 0;
	int y = 0;

	bool operator==(const Position& other) const{
		return (x == other.x && y == other.y);
	}

	bool operator<(const Position& other) const {
		return (x < other.x && y < other.y);
	}

	Position& operator+(const Position& other) {
		x += other.x;
		y += other.y;
		return *this;
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
				std::cout << index << "," << CurrentSprite()<<"\n";
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