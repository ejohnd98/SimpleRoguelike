#pragma once
#include <queue>
#include <vector>
#include "Constants.h"
#include "Components.h"

class Pathfinding
{
public:
	PathInfo GetPath(Position a, Position b, bool ignoreActors = true);
	PathInfo AStar(Position a, Position b, bool map[][MAX_MAP_SIZE], int w, int h, bool ignoreActors = true);

private:
	float GetHeuristic(Position a, Position b);
	bool NewOrShorterPath(Position a);
	bool ValidPos(Position a, int w, int h);

	bool visited[MAX_MAP_SIZE][MAX_MAP_SIZE];
	int dist[MAX_MAP_SIZE][MAX_MAP_SIZE];
	Position cameFrom[MAX_MAP_SIZE][MAX_MAP_SIZE];
};

