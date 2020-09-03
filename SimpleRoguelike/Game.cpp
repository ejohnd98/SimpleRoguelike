#include <stdio.h>
#include <iostream>
#include <assert.h>

#include "Game.h"
#include "ECS.h"

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
	//hardcoded first map 
	Entity mapEntity = ecs->CreateEntity();
	ecs->AddComponent(mapEntity, Map{});
	mapSystem->SetMap(mapEntity);

	std::vector<ActorType> actorTypes{
		{"Player", "you", {20, 10, 5, 1, 1, 10, 8}, 32, {32,33}, {30,30}},
		{"Ghost", "boo", {5, 6, 2, 1, 1, 20, 6}, 34, {34,35}, {40,40}},
		{"Skeleton", "bones", {10, 6, 3, 1, 1, 5, 6}, 37, {37,38}, {15,15}}
	};

	std::vector<PropType> propTypes{
		{"Door", "a door", 18, 20}
	};

	mapSystem->PlaceEntity(entityFactory->CreateActor(actorTypes[0], true), { 12,24 });
	mapSystem->PlaceEntity(entityFactory->CreateDoor(propTypes[0]), { 5,24 });
	mapSystem->PlaceEntity(entityFactory->CreateActor(actorTypes[2]), { 4,16 });
	mapSystem->PlaceEntity(entityFactory->CreateActor(actorTypes[2]), { 5,16 });
	mapSystem->PlaceEntity(entityFactory->CreateActor(actorTypes[2]), { 6,16 });
	mapSystem->PlaceEntity(entityFactory->CreateActor(actorTypes[1]), { 7,16 });

	fov->CalculateVisibleCells(playerSystem->GetPlayerEntity());

	//Game setup
	tickCounter = 0;
	state = GameState::RUNNING;
	return true;
}
void Game::CloseGame() {

}

bool Game::NotWaiting() {
	return !rendererSystem->AnimationPlaying() && state != GameState::WAITING_INPUT;
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
			if (nextCommand != Command::NONE && timeSinceLastInput <= 2 && timeSinceLastCommand >= 0) {
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