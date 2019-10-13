#include <stdio.h>
#include <iostream>
#include "GameLoop.h"
#include "Actor.h"
#include "Map.h"
#include "MapGenerator.h"
#include "Cell.h"

bool gameInitialized = false;
Map* currentMap = nullptr;
Actor* playerActor = nullptr;

void InitializeGame();
void TerminateGame();

GameLoop::GameLoop()
{
	InitializeGame();
}
GameLoop::~GameLoop()
{
	TerminateGame();
}

void InitializeGame() {
	MapGenerator* mapGen = new MapGenerator();
	currentMap = mapGen->GenerateMap(10,10);
	delete mapGen;
	
	playerActor = new Actor();
	playerActor->SetName("Hero");
	if (currentMap->PlaceActor(playerActor, 5, 5)) {
		std::cout << "Placed " << playerActor->GetName() << " (player) succesfully";
	}
	gameInitialized = true;
}

void TerminateGame() {
	gameInitialized = false;
	delete currentMap;
}

void GameLoop::AdvanceLoop() {
	if (gameInitialized) {
		//std::cout << "Game loop advanced\n";
	}
}

Map** GameLoop::GetMapPointer() {
	return &currentMap;
}
