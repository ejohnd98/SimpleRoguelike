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
#include "Pathfinder.h"
#include "FieldOfView.h"

#include "GameLog.h"

bool gameInitialized = false;
bool playerAlive = false;
int currentDepth = 1;
DungeonHolder* currentDungeon = nullptr;
Map* currentMap = nullptr;
Actor* playerActor = nullptr;
std::list<Command> pendingCommands = {};

GameLog* GameLog::instancePtr = 0;

void TerminateGame();
void TestMap();

GameLoop::GameLoop()
{
	InitializeGame();
}
GameLoop::~GameLoop()
{
	TerminateGame();
}

void GameLoop::InitializeGame() {
	
	DungeonGenerator* dungeonGen = new DungeonGenerator(); //create dungeon generator
	currentDungeon = dungeonGen->GenerateDungeon(4); //get a new dungeon
	currentMap = currentDungeon->GetMapAtDepth(1); //get first map in dungeon to use
	currentMap->SetGameLoop(this); //give map a reference to this gameloop
	delete dungeonGen; //deallocate dungeon generator
	//TestMap();
	//place player character (hardcoded for now)
	playerActor = new Actor("Hero", 32);
	if (currentMap->PlaceActor(playerActor, currentMap->GetWidth()/2, currentMap->GetHeight() / 2)) {
		playerActor->playerControlled = true;
		playerActor->SetFaction(1);
		std::cout << "Placed " << playerActor->GetName() << " (player) succesfully" << "\n";
	}
	playerAlive = true;
	playerActor->DealDamage(-50);

	FieldOfView::SetFOV(currentMap, playerActor); //calculate initial fov
	GameLog::instance()->AddLog("You delve into the dungeon!");
	gameInitialized = true;
}

void TerminateGame() {
	gameInitialized = false;
	delete currentDungeon;
}

void TestMap() {
	std::string mapLayout = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                            XX                      X     XX                      X     XX                      X     XXXXXXXXXXXXXXXXXXXXXXXXX XXXXXX      X       X             XX      X       X             XX      X       X             XXXX XXXX       X             XX                            XXXXXX XXXXXXXXXXXXXXXXXXXXXXXXX         X    X             XX         X         XXXX     XXX XXXXXXXX            XXXXX XX              X             XXXXXXXXXXXXXXXXXXXXXXXX XXXXXXX             X              XX                 X          XX             X              XX     X       X              XX    X X          X          XX   X   X     X              XX                            XX    X        X             XXXXXXX  X                  X  XX     XX      X         X   XXX    XX                      XX    X        X         X  X XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
	//std::string mapLayout = "XXXXXXXXXXXXXXXXXXXXX                  XX                  XX                  XX                  XX                  XX                  XX                  XX                  XX                  XX                  XX                  XX                  XX                  XX                  XX                  XX                  XX                  XX                  XXXXXXXXXXXXXXXXXXXXX";
	int n = mapLayout.length();
	int w = 30;
	int h = 30;
	for (int i = 0; i < n; i++) {
		int x = (i % w);
		int y = (i / w);
		if (mapLayout[i] == 'X') {
			currentMap->GetCell(x, y)->SetupCell(true);
		}
		else {
			currentMap->GetCell(x, y)->SetupCell(false);
		}
	}
	Actor* skel = new Actor("Skeleton", 34);
	if (currentMap->PlaceActor(skel, 1, 1)) {
		skel->SetFaction(0);
		std::cout << "Placed " << skel->GetName() << " succesfully" << "\n";
	}
	//currentMap->SetAllKnown(true);
	//currentMap->SetAllVisible(true);
}

void GameLoop::AdvanceLoop() {
	if (gameInitialized && playerAlive) {
		if (!pendingCommands.empty()) {

			//map debugging
			MapGenerator* mapGen = new MapGenerator();
			mapGen->GenerateMap(currentMap, currentMap->GetWidth(), currentMap->GetHeight());
			delete mapGen;

			//Coordinate test = Pathfinder::GetPath(playerActor->GetX(), playerActor->GetY(), 3, 8, currentMap);
			Command nextCom = pendingCommands.front();
			pendingCommands.pop_front();
			bool validCommand = false;
			if (playerActor != nullptr) {
				validCommand = playerActor->GiveCommand(nextCom);
			}
			if (validCommand) { //advance turn
				for (Actor* a : currentMap->actorList) {
					if (a != playerActor) {
						a->Act();
					}
				}
				FieldOfView::SetFOV(currentMap, playerActor);

				//debug stuff
				//currentMap->SetAllVisible(true); //keep map visible

			}
		}
	}
}

void GameLoop::GiveCommandFromMap(Command command) {
	switch (command) {
	case Command::NEXT_MAP:
		std::cout << "Gameloop received NEXT_MAP command\n";
		ChangeMap(GetNextMap(), true);
		currentDepth++;
		break;
	case Command::PREV_MAP:
		std::cout << "Gameloop received PREV_MAP command\n";
		ChangeMap(GetPrevMap(), false);
		currentDepth--;
		break;
	case Command::PLAYER_DIED:
		std::cout << "Gameloop received PLAYER_DIED command\n";
		playerAlive = false;
		playerActor->SetSprite(new Sprite(35));
		break;
	default:
		std::cout << "Gameloop received some other command\n";
		break;
	}
}

Map** GameLoop::GetMapPointer() {
	return &currentMap;
}

bool GameLoop::ChangeMap(Map* newMap, bool deeper) {
	if (newMap == nullptr) {
		printf("ERROR: ChangeMap called with nullptr");
		return false;
	}
	if (deeper && newMap->entrance == nullptr) {
		printf("ERROR: Descending to map without entrance");
		return false;
	}
	if (!deeper && newMap->exit == nullptr) {
		printf("ERROR: Ascending to map without exit");
		return false;
	}

	currentMap->RemoveActor(playerActor, false);
	currentMap = newMap;
	int x, y;
	if (deeper) {
		x = currentMap->entrance->GetX()+1;
		y = currentMap->entrance->GetY();
	} else {
		x = currentMap->exit->GetX()-1;
		y = currentMap->exit->GetY();
	}

	currentMap->PlaceActor(playerActor, x, y);
	std::cout << "Map changed!\n";
	return true; 
}

Map* GameLoop::GetPrevMap() {
	return currentDungeon->GetMapAtDepth(currentDepth - 1);
}
Map* GameLoop::GetNextMap() {
	return currentDungeon->GetMapAtDepth(currentDepth + 1);
}
Map* GameLoop::GetCurrentMap() {
	return currentDungeon->GetMapAtDepth(currentDepth);
}

Actor** GameLoop::GetPlayerPtr() {
	return &playerActor;
}

void GameLoop::GiveInput(Command command) {
	pendingCommands.push_back(command);
}
