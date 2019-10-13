#pragma once
#include "Map.h"

class GameRenderer
{
public:
	GameRenderer();
	~GameRenderer();

	void Render();
	void SetMap(Map** map);
};

