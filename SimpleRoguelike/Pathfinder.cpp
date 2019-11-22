#include <queue>
#include <vector>
#include <map>
#include <stdio.h>
#include <string>
#include <iostream>
#include <math.h>

#include "Pathfinder.h"
#include "Map.h"
#include "Cell.h"
#include "Coordinate.h"

int mapW;
int mapH;

struct Coord {
	int x;
	int y;
	Coord() {
		x = -1;
		y = -1;
	}
	Coord(int id) {
		int tempX = id;
		int tempY = 0;
		while (tempX >= mapW) {
			tempX -= mapW;
			tempY += 1;
		}
		x = tempX;
		y = tempY;
	}
	Coord(int xn, int yn) {
		x = xn;
		y = yn;
	}
	bool operator==(const Coord &rhs) const {
		return (x == rhs.x && y == rhs.y);
	}
	bool operator<(const Coord &rhs)  const {
		return (x < rhs.x && y < rhs.y);
	}
	int oneDimID() {
		int id = x;
		int tempY = y;
		while (tempY > 0) {
			id += mapW;
			tempY -= 1;
		}
		return id;
	}
};

int sourceX;
int sourceY;
int targetX;
int targetY;

std::map<int, Coord> cameFrom;
std::map<int, int> gScore;
std::map<int, float> hScore;
std::map<int, bool> isOpen;
std::map<int, bool> visited;

Coord GetLowestCoord();
float GetHCost(int x, int y);
void PrintPath();
Coord GetFirstStep();

Coordinate Pathfinder::GetPath(int sx, int sy, int tx, int ty, Map* map) {
	mapW = map->GetWidth();
	mapH = map->GetHeight();
	sourceX = sx;
	sourceY = sy;
	targetX = tx;
	targetY = ty;
	cameFrom.clear();
	gScore.clear();
	hScore.clear();
	isOpen.clear();
	visited.clear();

	for (int y = 0; y < mapH; y++) {
		for (int x = 0; x < mapW; x++) {
			Coord curr = Coord(x, y);
			gScore[curr.oneDimID()] = 9999999;
			hScore[curr.oneDimID()] = GetHCost(x, y);
			visited[curr.oneDimID()] = false;
		}
	}
	Coord source = Coord(sourceX, sourceY);
	gScore[source.oneDimID()] = 0;
	isOpen[source.oneDimID()] = true;
	visited[source.oneDimID()] = true;
	cameFrom[source.oneDimID()] = source;
	int openCount = 1;
	Coord curr;
	bool foundPath = false;

	while (openCount > 0) {
		curr = GetLowestCoord(); //get Coord in isOpen with lowest fscore
		//std::cout << "Looking at: " << curr.x << ", " << curr.y << "\n";
		if (curr.x == targetX && curr.y == targetY) { //found goal
			foundPath = true;
			break;
		}
		for (int i = 0; i < 4; i++) { //loop through adjacents
			int x = curr.x; int y = curr.y;
			switch (i) {
				case 0:
					x += 1; break;
				case 1:
					x -= 1; break;
				case 2:
					y += 1; break;
				case 3:
					y -= 1; break;
			}
			Coord adj = Coord(x, y); //current adjacent to look at
			if (visited[adj.oneDimID()] == false) {
				visited[adj.oneDimID()] = true;
				if (!map->PathBlocked(x, y)) { //and is not a wall, add to isOpen and calc new gScore
					gScore[adj.oneDimID()] = gScore[curr.oneDimID()] + 1;
					isOpen[adj.oneDimID()] = true;
					openCount++;
					cameFrom[adj.oneDimID()] = curr;
				}
			}
		}
		isOpen.erase(curr.oneDimID());
		openCount--;
	}
	//PrintPath();
	if (foundPath) {
		Coord r = GetFirstStep();
		return Coordinate(r.x, r.y);
	}
	else {
		return Coordinate();
	}
}


float GetHCost(int x1, int y1) {
	int x = targetX - x1;
	int y = targetY - y1;
	return 1.5*sqrt((x * x) + (y * y));
}

Coord GetLowestCoord() {
	Coord currentLow;
	float currentLowWeight = 99999999;
	std::map<int, bool>::iterator it;

	for (it = isOpen.begin(); it != isOpen.end(); it++)
	{
		Coord curr = Coord(it->first);
		float fScore = gScore[curr.oneDimID()] + hScore[curr.oneDimID()];
		if (fScore < currentLowWeight) {
			currentLowWeight = fScore;
			currentLow = curr;
		}
	}
	return currentLow;
}

void PrintPath() {
	int sourceID = Coord(sourceX, sourceY).oneDimID();
	Coord target = Coord(targetX, targetY);
	Coord curr = target;
	while (curr.oneDimID() != sourceID) {
		curr = cameFrom[curr.oneDimID()];
		std::cout << "Next Node: " << curr.x << ", " << curr.y << "\n";
	}
}

Coord GetFirstStep() {
	int sourceID = Coord(sourceX, sourceY).oneDimID();
	Coord target = Coord(targetX, targetY);
	Coord curr = target;
	while (cameFrom[curr.oneDimID()].oneDimID() != sourceID) {
		curr = cameFrom[curr.oneDimID()];
	}
	return curr;
}