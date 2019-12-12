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

Map* m;
int w;
int h;
int n;
int targx, targy;
int sourceID;
int targetID;

//keep outside getpath to avoid repeated declarations (got warning about using too much of the stack within function)
bool visited[Map::MAP_CELLS];
int dist[Map::MAP_CELLS];
int cameFrom[Map::MAP_CELLS];

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

Coordinate Pathfinder::GetPath(int sx, int sy, int tx, int ty, Map* map) { //given two coordinates and a map, constructs path between them if possible
	m = map;
	w = map->GetWidth();
	h = map->GetHeight();
	n = w * h;
	targx = tx;
	targy = ty;
	//all coordinates are represented by single integers (index if the map's 2d cell array were to be treated as a 1d array)
	sourceID = (sy * w) + sx;
	targetID = (ty * w) + tx;

	priority_queue<p, vector<p>, greater<p> > pq; //will be added to as cells are discovered

	for (int i = 0; i < n; i++) { //set initial values for arrays
		dist[i] = 999999;
		visited[i] = false;
		cameFrom[i] = i;
	}

	pq.push(make_pair(0, sourceID)); //add source cell to priority queue
	dist[sourceID] = 0;
	visited[sourceID] = true;

	int currID;
	bool pathFound = false;
	while (!pq.empty()) { //while cells exist in priority queue
		pair<float, int> curr = pq.top(); //get highest priority cell (lowest cost in pq)
		currID = curr.second; //get id from pair
		pq.pop(); //remove pair from pq

		if (currID == targetID) { //found target, so exit loop
			pathFound = true;
			break;
		}

		for (int i = 0; i < 4; i++) { //loop through adjacent cells
			int adjID = currID;
			switch (i) {
			case 0:
				adjID += 1; break; //x++
			case 1:
				adjID -= 1; break; //x--
			case 2:
				adjID += w; break; //y++
			case 3:
				adjID -= w; break; //y--
			}
			if (ValidCell(adjID)) { //check if cell is valid/not blocked
				if (!visited[adjID] || dist[currID] + 1 < dist[adjID]) { //if not yet visited or shorter path reached
					float newCost = dist[currID] + 1 + GetHeuristic(adjID); //calculate new cost to use for pq
					cameFrom[adjID] = currID; //set cell's parent
					dist[adjID] = dist[currID] + 1; //cost will be one more than curr/parent
					pq.push(make_pair(newCost, adjID)); //add to priority queue
					visited[adjID] = true; //set as visited
				}
			}
		}
	}

	if (pathFound) {//reconstruct path if found:
		currID = targetID;
		while (cameFrom[currID] != sourceID) { //follow cameFrom array from target back to once step before source
			currID = cameFrom[currID];
		}
		return Coordinate(currID % w, currID / w); //returns the first step in the path from source to target
	}
	else { //otherwise return an uninitialized coordinate
		return Coordinate();
	}
}

void Pathfinder::FillHeatMap(int sx, int sy, Map* map, int (*heatMap)[Map::MAP_WIDTH][Map::MAP_HEIGHT]) { //performs a breadth first search and fills heatmap with distances
	m = map;
	w = map->GetWidth();
	h = map->GetHeight();
	n = w * h;
	//all coordinates are represented by single integers (index if the map's 2d cell array were to be treated as a 1d array)
	sourceID = (sy * w) + sx;

	priority_queue<p, vector<p>, greater<p> > pq; //will be added to as cells are discovered

	for (int i = 0; i < n; i++) { //set initial values for arrays
		dist[i] = 999999;
		visited[i] = false;
	}
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			(*heatMap)[x][y] = -1;
		}
	}

	pq.push(make_pair(0, sourceID)); //add source cell to priority queue
	dist[sourceID] = 0;
	(*heatMap)[sx][sy] = 0;
	visited[sourceID] = true;
	
	int currID;
	bool pathFound = false;
	while (!pq.empty()) { //while cells exist in priority queue
		pair<float, int> curr = pq.top(); //get highest priority cell (lowest cost in pq)
		currID = curr.second; //get id from pair
		pq.pop(); //remove pair from pq

		for (int i = 0; i < 4; i++) { //loop through adjacent cells
			int adjID = currID;
			switch (i) {
			case 0:
				adjID += 1; break; //x++
			case 1:
				adjID -= 1; break; //x--
			case 2:
				adjID += w; break; //y++
			case 3:
				adjID -= w; break; //y--
			}
			if (ValidCell(adjID)) { //check if cell is valid/not blocked
				if (!visited[adjID]) { //if not yet visited or shorter path reached
					dist[adjID] = dist[currID] + 1; //cost will be one more than curr/parent
					(*heatMap)[adjID%w][adjID/w] = dist[adjID];
					pq.push(make_pair(dist[adjID], adjID)); //add to priority queue
					visited[adjID] = true; //set as visited
				}
			}
		}
	}
}