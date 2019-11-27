#include <queue>
#include <vector>
#include <stdio.h>
#include <string>
#include <iostream>
#include <math.h>
#include <SDL.h>

#include "Pathfinder.h"
#include "Map.h"
#include "Cell.h"
#include "Coordinate.h"

using namespace std;

typedef pair<float, int> p;

//optimizations:
//start: 21-22 ticks
//
Map* m;
int w;
int h;
int n;
int targx, targy;
int sourceID;
int targetID;

float GetHeuristic(int id) { //get distance from id cell to target (simple trig)
	int dx = targx - (id % w);
	int dy = targy - (id / w);
	return sqrt((dx * dx) + (dy * dy));
}

bool ValidCell(int id) { //checks if cell is in map, and not blocked
	int x = (id % w);
	int y = (id / w);
	return (m->ValidPos(x, y) && !m->PathBlocked(x, y));
}

Coordinate Pathfinder::GetPath(int sx, int sy, int tx, int ty, Map* map) {
	int start = SDL_GetTicks();
	int iterations = 0;

	m = map;
	w = map->GetWidth();
	h = map->GetHeight();
	n = w * h;
	targx = tx;
	targy = ty;
	sourceID = (sy * w) + sx;
	targetID = (ty * w) + tx;

	priority_queue<p, vector<p>, greater<p> > pq;
	bool visited[Map::MAP_CELLS];
	int dist[Map::MAP_CELLS];
	int cameFrom[Map::MAP_CELLS];

	for (int i = 0; i < n; i++) {
		dist[i] = 999999;
		visited[i] = false;
		cameFrom[i] = i;
	}

	pq.push(make_pair(0, sourceID));
	dist[sourceID] = 0;
	visited[sourceID] = true;

	int currID;
	bool pathFound = false;
	while (!pq.empty()) {
		iterations++;
		//get lowest cost cell from pq:
		pair<float, int> curr = pq.top();
		currID = curr.second;
		//cout << "looking at: " << currID % w << ", " << (int)(currID / w) << " with cost: " << curr.first << "\n";
		pq.pop();

		if (currID == targetID) {
			pathFound = true;
			break;
		}

		for (int i = 0; i < 4; i++) { //loop through adjacents
			int adjID = currID;
			switch (i) {
			case 0:
				adjID += 1; break;
			case 1:
				adjID -= 1; break;
			case 2:
				adjID += w; break;
			case 3:
				adjID -= w; break;
			}
			if (ValidCell(adjID)) { //check if cell is valid/not blocked
				float newCost = dist[currID] + 1 + GetHeuristic(adjID); //distance + heuristic
				if (!visited[adjID] || dist[currID] + 1 < dist[adjID]) { //if not yet visited or shorter path reached
					cameFrom[adjID] = currID;
					dist[adjID] = dist[currID] + 1;
					pq.push(make_pair(newCost, adjID)); //add to priority queue
					visited[adjID] = true; //have now visited
				}
			}
		}
	}

	if (pathFound) {
		//reconstruct path:
		currID = targetID;
		while (cameFrom[currID] != sourceID) { //go until 
			currID = cameFrom[currID];
		}

		cout << "Found from " << sx << ", " << sy << " to " << tx << ", " << ty << " with cost: " << dist[targetID] << "\n";
		int end = SDL_GetTicks();
		std::cout << "Took: " << (end - start) << " ms and " << iterations << " loops\n";

		return Coordinate(currID % w, currID / w);
	}
	else {
		cout << "Not found from " << sx << ", " << sy << " to " << tx << ", " << ty << "\n";
		int end = SDL_GetTicks();
		std::cout << "Took: " << (end - start) << " ms and " << iterations << " loops\n";

		return Coordinate(sx, sy);
	}
}