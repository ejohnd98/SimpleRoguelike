#pragma once
class Cell
{
public:
	Cell();
	~Cell();
	bool IsOccupied();
	void SetupCell(bool wall);
private:
	bool isWall;
};

