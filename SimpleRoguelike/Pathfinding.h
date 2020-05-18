#pragma once
#include <queue>
#include <vector>
#include "Constants.h"
#include "Components.h"

class Pathfinding
{
public:
	Position GetPath(Position a, Position b, std::shared_ptr<Map> map);
	Position AStar(Position a, Position b, bool map[][MAX_MAP_SIZE], int w, int h);

private:
	float GetHeuristic(Position a, Position b);
	bool NewOrShorterPath(Position a);
	bool ValidPos(Position a, int w, int h);

	bool visited[MAX_MAP_SIZE][MAX_MAP_SIZE];
	int dist[MAX_MAP_SIZE][MAX_MAP_SIZE];
	Position cameFrom[MAX_MAP_SIZE][MAX_MAP_SIZE];
};

