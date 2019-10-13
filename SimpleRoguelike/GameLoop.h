#pragma once
class GameLoop
{
public:
	GameLoop();
	~GameLoop();
	void AdvanceLoop();
	class Map** GetMapPointer();
};

