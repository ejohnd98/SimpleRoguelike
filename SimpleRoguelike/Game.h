#pragma once
#include <stdio.h>
#include <iostream>
#include <list>

#include "Commands.h"
#include "ECS.h"

class Game
{
public:
	Game();
	~Game();
	bool InitGame();
	void CloseGame();

	void Advance();
	void GiveInput(Command command);

	enum class GameState {
		ANIMATING,
		WAITING_INPUT,
		RUNNING,
		ADVANCE_TURN
	};

private:
	GameState state;
	std::list<Command> pendingCommands = {};
};

