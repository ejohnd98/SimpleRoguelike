#pragma once

class GameRenderer
{
public:
	GameRenderer();
	~GameRenderer();

	void Render();
private:
	std::shared_ptr<class Game> game;
};

