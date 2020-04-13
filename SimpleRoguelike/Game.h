#pragma once
#include <stdio.h>
#include <iostream>
#include <queue>

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
	bool AcceptingInput();

	enum class GameState {
		ANIMATING,
		WAITING_INPUT,
		RUNNING,
		ADVANCE_TURN
	};

private:
	GameState state;
	std::queue<Command> pendingCommands = {};
};

