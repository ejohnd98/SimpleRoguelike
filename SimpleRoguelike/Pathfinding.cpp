#include <stdio.h>

#include <iostream>

#include "Pathfinding.h"
#include "ECS.h"

using namespace std;
typedef pair<float, Position> p;

Position Pathfinding::GetPath(Position a, Position b, std::shared_ptr<Map> map) { //change to take in a bool array, or have a separate function for that
	return AStar(a, b, map->cells, map->width, map->height);
}

Position Pathfinding::AStar(Position a, Position b, bool map[MAX_MAP_SIZE][MAX_MAP_SIZE], int w, int h){ //map arrays are accessed [y][x]

	//init
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			dist[y][x] = 999999;
			visited[y][x] = false;
			cameFrom[y][x] = { -1,-1 };
		}
	}
	priority_queue<p, vector<p>, greater<p> > pq;

	pq.push(make_pair(0, a)); //add start to queue
	dist[a.y][a.x] = 0;
	visited[a.y][a.x] = true;
	cameFrom[a.y][a.x] = a;

	Position curr;
	bool pathFound = false;
	while (!pq.empty()) {
		curr = pq.top().second; //get position on top of queue
		pq.pop();

		if (curr == b) {
			pathFound = true;
			break;
		}

		for (int i = 0; i < 4; i++) { //loop through adjacent cells 
			Position adj = { curr.x, curr.y };
			switch (i) {
			case 0:
				adj.x++; break; //only add if within bounds
			case 1:
				adj.x--; break;
			case 2:
				adj.y++; break;
			case 3:
				adj.y--; break;
			}
			if (ValidPos(adj, w, h) && !map[adj.y][adj.x]) { //if space is free (true = wall)
				if (NewOrShorterPath(adj)) { 
					cameFrom[adj.y][adj.x] = curr; //set path followed to get here
					dist[adj.y][adj.x] = dist[curr.y][curr.x] + 1; //dist is one more than parent (as we are working with grids)
					float priority = dist[adj.y][adj.x] + GetHeuristic(adj, b); //set priority to dist + heuristic
					pq.push(make_pair(priority, adj)); //add to queue
					visited[adj.y][adj.x] = true; //set as visited
				}
			}
		}

	}
	if (pathFound) { //reconstruct path
		curr = b;
		while (!(cameFrom[curr.y][curr.x] == a)) {
			curr = cameFrom[curr.y][curr.x];
			if (curr == Position{ -1, -1 }) {
				std::cout << "ERROR in pathfinding reconstruction\n";
			}
		}
		return curr;
	}
	else {
		std::cout << "Path not found from: " << a.x << ", " << a.y << " to " << b.x << ", " << b.y << "\n";
		return a;
	}


}

float Pathfinding::GetHeuristic(Position a, Position b) { //get distance between two positions
	int dx = b.x - a.x;
	int dy = b.y - a.y;
	return sqrt((dx * dx) + (dy * dy));
}

bool Pathfinding::NewOrShorterPath(Position a) {
	return (!visited[a.y][a.x] || dist[a.y][a.x] + 1 < dist[a.y][a.x]);
}

bool Pathfinding::ValidPos(Position a, int w, int h) {
	return (a.x >= 0 && a.x < w && a.y >= 0 && a.y < h);
}