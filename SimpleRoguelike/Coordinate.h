#pragma once
class Coordinate
{
public:
	int x;
	int y;
	bool init = false;

	Coordinate() {
		x = -1;
		y = -1;
		init = false;
	}
	Coordinate(int x1, int y1) {
		x = x1;
		y = y1;
		init = true;
	}
	void SetValues(Coordinate c) {
		x = c.x;
		y = c.y;
	}
	bool IsCoord(int x1, int y1) {
		return (x1 == x && y1 == y);
	}
	bool IsCoord(Coordinate c) {
		return IsCoord(c.x, c.y);
	}
	
};

