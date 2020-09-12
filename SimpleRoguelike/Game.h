#pragma once
#include <stdio.h>
#include <iostream>
#include <queue>
#include "Commands.h"

class Game
{
public:
	Game();
	~Game();
	bool InitGame();
	void CloseGame();
	void InitMapTest();

	bool StillProcessing();
	bool GeneratingMap();
	void Advance(bool sameStep = false);
	void GiveInput(Command command);
	void ClearHeldInput();
	bool AcceptingInput();
	void AdvanceTurn();

	enum class GameState {
		ANIMATING,
		WAITING_INPUT,
		RUNNING,
		MAP_GEN
	};

	std::shared_ptr<class MapGenerator> mapGen;

private:
	GameState state;
	Command nextCommand = Command::NONE;
	
};

