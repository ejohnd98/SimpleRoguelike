#include <stdio.h>
#include <iostream>
#include <assert.h>

#include "Game.h"
#include "ECS.h"
#include "JSONHandler.h"
#include "MapGenerator.h"
#include "DungeonGenerator.h"
#include "EntityFactory.h"
#include "RandomUtil.h"

//variables
int timeSinceLastCommand = 0;
int timeSinceLastInput = 0;
int tickCounter;

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr<TurnSystem> turnSystem;
extern std::shared_ptr<MapSystem> mapSystem;
extern std::shared_ptr<DungeonSystem> dungeonSystem;
extern std::shared_ptr<PlayerSystem> playerSystem;
extern std::shared_ptr<AISystem> aiSystem;
extern std::shared_ptr<RendererSystem> rendererSystem;
extern std::shared_ptr<AnimationSystem> animationSystem;
extern std::shared_ptr<LogSystem> logSystem;
extern std::shared_ptr<EntityFactory> entityFactory;
extern std::shared_ptr<FieldOfView> fov;
extern std::shared_ptr<RandomUtil> randomUtil;
extern std::shared_ptr<MemorySystem> memorySystem;

Game::Game(){
	InitGame();
}
Game::~Game(){
	CloseGame();
}

bool Game::InitGame() {
	//CREATE DUNGEON INFO STRUCT TO STORE STUFF LIKE MAP SIZES, DEPTH, THEME, etc...

	//create dungeon object
	std::shared_ptr<Dungeon> dungeon = std::make_shared<Dungeon>();
	dungeonSystem->SetDungeon(dungeon);

	dunGen = std::make_shared<DungeonGenerator>(randomUtil->GetRandomInt(0,9000000));
	dunGen->Begin(dungeon, 10);

	//Game setup
	tickCounter = 0;
	state = GameState::MAP_GEN;
	return true;
}
void Game::CloseGame() {
	//mapSystem->Clear();
	mapGen = nullptr;
}

void Game::InitMapTest() {
	Position playerPos = mapSystem->map->entrance + Position{1, 0};

	mapSystem->PlaceEntity(entityFactory->CreateActor(actorTypes[0], true), playerPos);

	fov->CalculateVisibleCells(playerSystem->GetPlayerEntity());
	memorySystem->CleanUpEntities();
	Item testItem;
	testItem.name = "Test Item";
	testItem.sprite = 49;
	testItem.tileset = MAIN_TILESET;
	testItem.empty = false;

	mapSystem->PlaceItemAt(testItem, mapSystem->map->entrance + Position{ 1,1 });
}

bool Game::StillProcessing() {
	return state == GameState::RUNNING && playerSystem->GetPlayerEntity() != NULL_ENTITY;
}

bool Game::GeneratingMap() {
	return state == GameState::MAP_GEN;
}

void Game::Advance(bool sameStep) {
	if (rendererSystem->AnimationPlaying()) {
		state = GameState::ANIMATING;
	}
	int tickLimit = 0; //limits amount of ticks that can be advanced in one function call (prevent endless loops)

	switch (state) {
		case GameState::ANIMATING:
			if (rendererSystem->AnimationPlaying()) {
				break;
			}
			else {
				fov->CalculateVisibleCells(playerSystem->GetPlayerEntity());
				state = GameState::RUNNING;
			}
		case GameState::RUNNING:
			while (!turnSystem->EntityCanAct() && tickLimit++ < 100) {
				turnSystem->DecreaseDebt(10);
				tickCounter++;
				if (tickCounter % 10 == 0) {
					AdvanceTurn();
					memorySystem->CleanUpEntities();
					//animationSystem->PlayPendingAnimations();
					logSystem->PushLogs();
				}
			}

			if (turnSystem->PlayerActsNext()) {
				fov->CalculateVisibleCells(playerSystem->GetPlayerEntity()); //refresh fov before player input
				animationSystem->PlayPendingAnimations(); //play any pending animations before giving player control
				logSystem->PushLogs();
				state = GameState::WAITING_INPUT;
				break;
			}
			else if (turnSystem->PeekNextActor() != NULL_ENTITY) {
				//Get entity
				Entity entity = turnSystem->PeekNextActor();
				turnSystem->PopNextActor();
				assert(ecs->HasComponent<Active>(entity));

				//Let entity act
				ecs->AddComponent<ActiveAIEntity>(entity, {});
				aiSystem->DetermineAction();
				ecs->RemoveComponent<ActiveAIEntity>(entity); 
				break;
			}
			else { 
				break;
			}
			 
		case GameState::WAITING_INPUT:
			if (nextCommand != Command::NONE && timeSinceLastInput <= 2 && timeSinceLastCommand >= 6) {
				Entity entity = turnSystem->PeekNextActor();
				if (playerSystem->DetermineAction(nextCommand)) { //only remove player from queue if they performed an action
					turnSystem->PopNextActor();
					state = GameState::RUNNING;
					//animationSystem->PlayPendingAnimations(); //play animation from player separately from enemies
					state = GameState::ANIMATING;
				}
				nextCommand = Command::NONE;
				timeSinceLastCommand = 0;
			}
			break;
		case GameState::MAP_GEN:
			if (dunGen->IsFinished()) {
				InitMapTest();
				state = GameState::RUNNING;
			}
			else {
				dunGen->GenerationStep();
			}
			break;

	}
	if (!sameStep) {
		timeSinceLastInput++;
		timeSinceLastCommand++;
	}
}

void Game::GiveInput(Command command) {
	nextCommand = command;
	timeSinceLastInput = 0;
}

void Game::ClearHeldInput() {
	nextCommand = Command::NONE;
}

bool Game::AcceptingInput() {
	return true;
}

void Game::AdvanceTurn() {
	//increment turns counter
	//do turn effects like poison
	std::cout << "Advancing turn\n";
}