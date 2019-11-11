#include "Pathfinder.h"
#include "Map.h"
#include "Cell.h"
#include <queue>
#include <vector>
#include <map>
#include <stdio.h>
#include <string>
#include <iostream>




struct Coord {
	int x;
	int y;
	Coord() {
		x = -1;
		y = -1;
	}
	Coord(int x1, int y1) {
		x = x1;
		y = y1;
	}
	bool operator==(const Coord &rhs) const {
		return (x == rhs.x && y == rhs.y);
	}
	bool operator<(const Coord &rhs)  const {
		return (x < rhs.x && y < rhs.y);
	}
};

Coord GetLowestCoord(std::map<Coord, int>* gScore, std::map<Coord, int>* hScore, std::map<Coord, bool>* isOpen);
int GetHCost(int x1, int y1, int x2, int y2);

Pathfinder::Pathfinder()
{
}


Pathfinder::~Pathfinder()
{
}

//BASICALLY NEED TO STOP USING STRUCT AS A KEY I THINK???

void Pathfinder::GetPath(int x1, int y1, int x2, int y2, Map* map) {
	return;
	const int w = map->GetWidth();
	const int h = map->GetHeight();
	std::map<Coord, Coord> cameFrom;
	std::map<Coord, int> gScore;
	std::map<Coord, int> hScore;
	std::map<Coord, bool> isOpen;
	std::map<Coord, bool> visited;

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			Coord curr = Coord(x, y);
			gScore[curr] = 9999999;
			hScore[curr] = GetHCost(x, y, x2, y2);
			visited[curr] = false;
		}
	}
	Coord source = Coord(x1, y1);
	gScore[source] = 0;
	isOpen[source] = true;
	visited[source] = true;
	cameFrom[source] = source;
	int openCount = 1;
	Coord curr;

	while (openCount > 0) {
		curr = GetLowestCoord(&gScore, &hScore, &isOpen); //get Coord in isOpen with lowest fscore
		std::cout << "Looking at: " << curr.x << ", " << curr.y << "\n";
		if (curr.x == x2 && curr.y == y2) { //found goal
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
			if (visited[adj] == false) { //if not in isOpen DOES NOT WORK. NEED OTHER WAY TO TELL IF IT"S BEEN VISITED YET
				visited[adj] = true;
				if (!map->IsWall(x, y)) { //and is not a wall, add to isOpen and calc new gScore
					gScore[adj] = gScore[curr] + 1;
					isOpen[adj] = true;
					openCount++;
					cameFrom[adj] = curr;
				}
			}
		}
		isOpen.erase(curr);
		openCount--;
	}

	std::cout << "Found: " << curr.x << ", " << curr.y << "\n";

}


int GetHCost(int x1, int y1, int x2, int y2) {
	return (x2 - x1) + (y2 - y1);
}

Coord GetLowestCoord(std::map<Coord, int>* gScore, std::map<Coord, int>* hScore, std::map<Coord, bool>* isOpen) {
	Coord currentLow;
	int currentLowWeight = 99999999;
	std::map<Coord, bool>::iterator it;

	for (it = (*isOpen).begin(); it != (*isOpen).end(); it++)
	{
		Coord curr = it->first;
		int fScore = (*gScore)[curr] + (*hScore)[curr];
		if (fScore < currentLowWeight) {
			currentLowWeight = fScore;
			currentLow = curr;
		}
	}
	return currentLow;
}
