#pragma once

class GameRenderer
{
public:
	GameRenderer();
	~GameRenderer();

	void Render();
	void SetGameLoop(class GameLoop* loop);
};

