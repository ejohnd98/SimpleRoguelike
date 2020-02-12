#include <stdio.h>
#include <iostream>
#include <list>
#include <string>

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
#include "DatabaseReader.h"
#include "GameLog.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

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
	currentDungeon = dungeonGen->GenerateDungeon(3); //get a new dungeon
	currentDepth = 1;
	currentMap = GetCurrentMap(); //get first map in dungeon to use
	currentMap->SetGameLoop(this); //give map a reference to this gameloop
	delete dungeonGen; //deallocate dungeon generator
	//TestMap();

	//--------------------------//
	//database development tests//
	//--------------------------//

	rapidjson::Document doc = DatabaseReader::ReadJSON("database/test.json");
	Actor* actor = DatabaseReader::DocumentToActor(doc);
	std::cout << "Actor name and sprite: " << actor->GetName() << ", " << actor->GetSprite()->GetIndex() << "\n";

	//--------------------------//


	//place player character (hardcoded for now)
	Coordinate playerStartPos = currentMap->GetPosAroundStairs(true);
	playerActor = new Actor("Hero", 32);
	if (currentMap->PlaceActor(playerActor, playerStartPos.x, playerStartPos.y)) {
		playerActor->playerControlled = true;
		playerActor->SetFaction(1);
		std::cout << "Placed " << playerActor->GetName() << " (player) succesfully" << "\n";
	}
	playerAlive = true;
	playerActor->DealDamage(-90);

	FieldOfView::SetFOV(currentMap, playerActor); //calculate initial fov
	GameLog::instance()->AddLog("You delve into the dungeon!");
	gameInitialized = true;
}

void TerminateGame() {
	gameInitialized = false;
	delete currentDungeon;
}

void TestMap() {
	std::string mapLayout = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                              XX                      X     XX                      X     XX                      X     XXXXXXXXXXXXXXXXXXXXXXXXX XXXXXX      X       X             XX      X       X             XX      X       X             XXXX XXXX       X             XX                            XXXXXX XXXXXXXXXXXXXXXXXXXXXXXXX         X    X             XX         X         XXXX     XXX XXXXXXXX            XXXXX XX              X             XXXXXXXXXXXXXXXXXXXXXXXX XXXXXXX             X              XX                 X          XX             X              XX     X       X              XX    X X          X          XX   X   X     X              XX                            XX    X        X             XXXXXXX  X                  X  XX     XX      X         X   XXX    XX                      XX    X        X         X  X XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
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
				GameLog::instance()->AdvanceTurn();

				//debug stuff
				//currentMap->SetAllKnown(true); //keep map visible
				//currentMap->SetAllVisible(true); //keep map visible

			}
		}
	}
}

void GameLoop::GiveCommandFromMap(Command command) { //called from the map to pass a command to the game
	switch (command) {
	case Command::NEXT_MAP:
		std::cout << "Gameloop received NEXT_MAP command\n";
		
		ChangeMap(GetNextMap(), true);
		currentDepth++;
		GameLog::instance()->AddLog("You descend to level " + std::to_string(currentDepth) + " of the dungeon");
		break;
	case Command::PREV_MAP:
		std::cout << "Gameloop received PREV_MAP command\n";
		
		ChangeMap(GetPrevMap(), false);
		currentDepth--;
		GameLog::instance()->AddLog("You ascend to level " + std::to_string(currentDepth) + " of the dungeon");
		break;
	case Command::PLAYER_DIED:
		if (playerAlive) {
			GameLog::instance()->AddLog("Your adventure ends here...");
			playerAlive = false;
			playerActor->SetSprite(new Sprite(35));
		}
		break;
	case Command::WON_THE_GAME:
		GameLog::instance()->AddLog("Congratulations, you succeeded in your quest!");
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
	Coordinate newPlayerPos = currentMap->GetPosAroundStairs(deeper);
	
	currentMap->PlaceActor(playerActor, newPlayerPos.x, newPlayerPos.y);
	currentMap->SetGameLoop(this);
	std::cout << "Map changed!\n";
	return true; 
}
int GameLoop::GetCurrentDepth() {
	return currentDepth;
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
