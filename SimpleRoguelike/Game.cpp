#include <stdio.h>
#include <iostream>
#include <assert.h>

#include "Game.h"
#include "ECS.h"
#include "JSONHandler.h"
#include "MapGenerator.h"

//variables
int timeSinceLastCommand = 0;
int timeSinceLastInput = 0;
int tickCounter;

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr<TurnSystem> turnSystem;
extern std::shared_ptr<MapSystem> mapSystem;
extern std::shared_ptr<PlayerSystem> playerSystem;
extern std::shared_ptr<AISystem> aiSystem;
extern std::shared_ptr<RendererSystem> rendererSystem;
extern std::shared_ptr<AnimationSystem> animationSystem;
extern std::shared_ptr<LogSystem> logSystem;
extern std::shared_ptr<EntityFactory> entityFactory;
extern std::shared_ptr<FieldOfView> fov;

Game::Game(){
	InitGame();
}
Game::~Game(){
	CloseGame();
}

bool Game::InitGame() {
	//generate map
	std::shared_ptr<Map> map = std::make_shared<Map>();
	mapSystem->SetMap(map);

	mapGen = std::make_shared<MapGenerator>(999);
	mapGen->Begin(map, 50, 50);

	//Game setup
	tickCounter = 0;
	state = GameState::MAP_GEN;
	return true;
}
void Game::CloseGame() {

}

void Game::InitMapTest() {
	std::vector<ActorType> actorTypes{
		{"Player", "you", {29, 6, 5, 1, 1, 10, 8}, 32, {32,33}, {40,40}},
		{"Ghost", "boo", {5, 6, 2, 1, 1, 20, 6}, 34, {34,35}, {25,25}},
		{"Skeleton", "bones", {10, 6, 3, 1, 1, 5, 6}, 37, {37,38}, {60,60}}
	};

	mapSystem->PlaceEntity(entityFactory->CreateActor(actorTypes[0], true), { 12,24 });
	mapSystem->PlaceEntity(entityFactory->CreateActor(actorTypes[1]), { 7,15 });
	mapSystem->PlaceEntity(entityFactory->CreateActor(actorTypes[2]), { 4,16 });

	mapSystem->PlaceEntity(entityFactory->CreateActor(actorTypes[2]), { 13,16 });
	mapSystem->PlaceEntity(entityFactory->CreateActor(actorTypes[1]), { 16,18 });
	mapSystem->PlaceEntity(entityFactory->CreateActor(actorTypes[2]), { 19,27 });

	mapSystem->PlaceEntity(entityFactory->CreateActor(actorTypes[1]), { 22,23 });
	mapSystem->PlaceEntity(entityFactory->CreateActor(actorTypes[1]), { 23,23 });
	mapSystem->PlaceEntity(entityFactory->CreateActor(actorTypes[2]), { 24,23 });

	fov->CalculateVisibleCells(playerSystem->GetPlayerEntity());
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
			while (!turnSystem->EntityCanAct() && tickLimit++ < 10) {
				turnSystem->DecreaseDebt(10);
				tickCounter++;
				if (tickCounter % 10 == 0) {
					AdvanceTurn();
					animationSystem->PlayPendingAnimations();
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
					animationSystem->PlayPendingAnimations(); //play animation from player
					state = GameState::ANIMATING;
				}
				nextCommand = Command::NONE;
				timeSinceLastCommand = 0;
			}
			break;
		case GameState::MAP_GEN:
			if (mapGen->IsFinished()) {
				InitMapTest();
				state = GameState::RUNNING;
			}
			else {
				mapGen->GenerationStep();
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