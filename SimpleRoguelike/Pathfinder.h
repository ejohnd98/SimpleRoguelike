#pragma once
class Pathfinder
{
public:
	Pathfinder();
	~Pathfinder();
	static void GetPath(int x1, int y1, int x2, int y2, class Map* map);
};