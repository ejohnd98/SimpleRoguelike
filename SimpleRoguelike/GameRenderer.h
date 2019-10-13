#pragma once
#include "Map.h"

class GameRenderer
{
public:
	GameRenderer();
	~GameRenderer();

	bool Init();
	void Render();
	void SetMap(Map** map);
};

