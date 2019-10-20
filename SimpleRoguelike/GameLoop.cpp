#include <stdio.h>
#include <iostream>
#include <list>
#include "GameLoop.h"
#include "Actor.h"
#include "DungeonHolder.h"
#include "Map.h"
#include "MapGenerator.h"
#include "Cell.h"
#include "Commands.h"
#include "Sprite.h"
#include "Prop.h"

bool gameInitialized = false;
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
	MapGenerator* mapGen = new MapGenerator();
	currentMap = mapGen->GenerateMap(10,10);
	currentMap->SetGameLoop(this);
	delete mapGen;
	
	playerActor = new Actor("Hero", new Sprite(2));
	if (currentMap->PlaceActor(playerActor, 5, 5)) {
		std::cout << "Placed " << playerActor->GetName() << " (player) succesfully" << "\n";
	}
	playerActor->SetMapRef(currentMap);

	Actor* enemy = new Actor("Ghost", new Sprite(3));
	if (currentMap->PlaceActor(enemy, 7, 3)) {
		std::cout << "Placed " << enemy->GetName() << " succesfully" << "\n";
	}
	enemy->SetMapRef(currentMap);

	Actor* enemy2 = new Actor("Big Ghost", new Sprite(3));
	if (currentMap->PlaceActor(enemy2, 9, 3)) {
		std::cout << "Placed " << enemy2->GetName() << " succesfully" << "\n";
	}
	enemy->SetMapRef(currentMap);

	Prop* exit = new Prop("Stairs", new Sprite(6), Command::NEXT_MAP);
	if (currentMap->PlaceProp(exit, 2, 3)) {
		std::cout << "Placed " << exit->GetName() << " succesfully" << "\n";
	}
	exit->SetMapRef(currentMap);

	Prop* exit2 = new Prop("StairsBack", new Sprite(6), Command::PREV_MAP);
	if (currentMap->PlaceProp(exit2, 7, 8)) {
		std::cout << "Placed " << exit2->GetName() << " succesfully" << "\n";
	}
	exit2->SetMapRef(currentMap);

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
		break;
	case Command::PREV_MAP:
		std::cout << "Gameloop received PREV_MAP command\n";
		ChangeMap(GetPrevMap());
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
	std::cout << "Map changed! (not really)\n";
	return true; 
}

Map* GameLoop::GetPrevMap() {
	return nullptr;
}
Map* GameLoop::GetNextMap() {
	return nullptr;
}

void GameLoop::GiveInput(Command command) {
	pendingCommands.push_back(command);
}
