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

	bool StillProcessing();
	void Advance(bool sameStep = false);
	void GiveInput(Command command);
	void ClearHeldInput();
	bool AcceptingInput();
	void AdvanceTurn();

	enum class GameState {
		ANIMATING,
		WAITING_INPUT,
		RUNNING
	};

private:
	GameState state;
	Command nextCommand = Command::NONE;
};

