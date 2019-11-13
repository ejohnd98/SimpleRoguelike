#pragma once
class Pathfinder
{
public:
	struct Coordinate {
		int x;
		int y;
		Coordinate(int x1, int y1) {
			x = x1;
			y = y1;
		}
	};

	static Coordinate GetPath(int x1, int y1, int x2, int y2, class Map* map);
	
};