#include <stdio.h>
#include "GameLoop.h"
#include "Map.h"
#include "Cell.h"
#include <iostream>

bool gameInitialized = false;
Map* currentMap = NULL;

void InitializeGame();

GameLoop::GameLoop()
{
	InitializeGame();
}
GameLoop::~GameLoop()
{
}

void InitializeGame() {
	currentMap = new Map();
	
	for (int y = 0; y < 10; y++) {
		for (int x = 0; x < 10; x++) {
			if (currentMap->GetCell(x, y)->IsOccupied()) {
				std::cout << "X";
			}
			else {
				std::cout << ".";
			}
			
		}
		std::cout << "\n";
	}
	gameInitialized = true;
}

void GameLoop::AdvanceLoop() {
	if (gameInitialized) {
		//std::cout << "Game loop advanced\n";
	}
}