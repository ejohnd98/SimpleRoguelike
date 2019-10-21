#include <stdio.h>
#include <iostream>
#include <list>
#include "GameLoop.h"
#include "Actor.h"
#include "DungeonGenerator.h"
#include "DungeonHolder.h"
#include "Map.h"
#include "MapGenerator.h"
#include "Cell.h"
#include "Commands.h"
#include "Sprite.h"
#include "Prop.h"

bool gameInitialized = false;
int currentDepth = 1;
DungeonHolder* currentDungeon = nullptr;
Map* currentMap = nullptr;
Actor* playerActor = nullptr;
std::list<Command> pendingCommands = {};

void TerminateGame();

GameLoop::GameLoop()
{
	InitializeGame();
}
GameLoop::~GameLoop()
{
	TerminateGame();
}

void GameLoop::InitializeGame() {
	DungeonGenerator* dungeonGen = new DungeonGenerator();
	currentDungeon = dungeonGen->GenerateDungeon(4);
	currentMap = currentDungeon->GetMapAtDepth(1);
	currentMap->SetGameLoop(this);
	delete dungeonGen;
	
	playerActor = new Actor("Hero", 2);
	if (currentMap->PlaceActor(playerActor, 5, 5)) {
		std::cout << "Placed " << playerActor->GetName() << " (player) succesfully" << "\n";
	}

	gameInitialized = true;
}

void TerminateGame() {
	gameInitialized = false;
	delete currentMap;
}

void GameLoop::AdvanceLoop() {
	if (gameInitialized) {
		if (!pendingCommands.empty()) {
			Command nextCom = pendingCommands.front();
			pendingCommands.pop_front();
			bool validCommand = false;
			if (playerActor != nullptr) {
				validCommand = playerActor->GiveCommand(nextCom);
			}
			if (validCommand) {
				for (Actor* a : currentMap->actorList) {
					if (a != playerActor) {
						a->Act();
					}
				}
			}
			
		}
		//std::cout << "Game loop advanced\n";
	}
}

void GameLoop::GiveCommandFromMap(Command command) {
	switch (command) {
	case Command::NEXT_MAP:
		std::cout << "Gameloop received NEXT_MAP command\n";
		ChangeMap(GetNextMap());
		currentDepth++;
		break;
	case Command::PREV_MAP:
		std::cout << "Gameloop received PREV_MAP command\n";
		ChangeMap(GetPrevMap());
		currentDepth--;
		break;
	default:
		std::cout << "Gameloop recievd some other command\n";
		break;
	}
}

Map** GameLoop::GetMapPointer() {
	return &currentMap;
}

bool GameLoop::ChangeMap(Map* newMap) {
	if (newMap == nullptr) {
		printf("ERROR: ChangeMap called with nullptr");
	}
	currentMap->RemoveActor(playerActor, false);
	currentMap = newMap;
	currentMap->PlaceActor(playerActor, 7, 8);
	std::cout << "Map changed!\n";
	return true; 
}

Map* GameLoop::GetPrevMap() {
	return currentDungeon->GetMapAtDepth(currentDepth - 1);
}
Map* GameLoop::GetNextMap() {
	return currentDungeon->GetMapAtDepth(currentDepth + 1);
}

void GameLoop::GiveInput(Command command) {
	pendingCommands.push_back(command);
}
