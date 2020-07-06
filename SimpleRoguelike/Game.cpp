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

	Entity player = ecs->CreateEntity();
	ecs->AddComponent(player, Actor{0});
	ecs->AddComponent(player, PlayerControlled{});
	ecs->AddComponent(player, Position{});
	ecs->AddComponent(player, Active{});
	ecs->AddComponent(player, Renderable{0});
	ecs->AddComponent(player, Stats{20,6,1,10,10});
	Sprite anim[] = { 32,33 };
	animationSystem->AddIdleAnim(player, anim, 2, 30);

	Entity enemy = ecs->CreateEntity();
	ecs->AddComponent(enemy, Actor{0});
	ecs->AddComponent(enemy, AIControlled{});
	ecs->AddComponent(enemy, Position{});
	ecs->AddComponent(enemy, Active{});
	ecs->AddComponent(enemy, Renderable{0});
	ecs->AddComponent(enemy, Stats{5,4,3,5,10});
	Sprite anim2[] = { 37,38 };
	animationSystem->AddIdleAnim(enemy, anim2, 2, 60);

	Entity enemy2 = ecs->CreateEntity();
	ecs->AddComponent(enemy2, Actor{ 0 });
	ecs->AddComponent(enemy2, AIControlled{});
	ecs->AddComponent(enemy2, Position{});
	ecs->AddComponent(enemy2, Active{});
	ecs->AddComponent(enemy2, Renderable{ 0 });
	ecs->AddComponent(enemy2, Stats{ 5,4,3,5,10 });
	animationSystem->AddIdleAnim(enemy2, anim2, 2, 60);

	Entity enemy3 = ecs->CreateEntity();
	ecs->AddComponent(enemy3, Actor{ 0 });
	ecs->AddComponent(enemy3, AIControlled{});
	ecs->AddComponent(enemy3, Position{});
	ecs->AddComponent(enemy3, Active{});
	ecs->AddComponent(enemy3, Renderable{ 0 });
	ecs->AddComponent(enemy3, Stats{ 5,4,3,5,10 });
	animationSystem->AddIdleAnim(enemy3, anim2, 2, 60);

	mapSystem->PlaceEntity(player, { 12,24 });
	mapSystem->PlaceEntity(enemy, { 4,16 });
	mapSystem->PlaceEntity(enemy2, { 32,19 });
	mapSystem->PlaceEntity(enemy3, { 18,9 });

	fov->CalculateVisibleCells(playerSystem->GetPlayerEntity());

	//Game setup
	tickCounter = 0;
	state = GameState::RUNNING;
	return true;
}
void Game::CloseGame() {

}

void Game::Advance() {
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
				animationSystem->PlayPendingAnimations(); //play pending animations before giving control back to player
				state = GameState::WAITING_INPUT;
			}
			else if (turnSystem->EntityCanAct()) {
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
			if (timeSinceLastInput > 7 || timeSinceLastCommand < 5) { //Will probably need to change these numbers later to match with framerate
				nextCommand = Command::NONE;
			}
			if (nextCommand!=Command::NONE) {
				Entity entity = turnSystem->PeekNextActor();
				if (playerSystem->DetermineAction(nextCommand)) { //only remove player from queue if they performed an action
					//std::cout << "---Player acting---\n";
					turnSystem->PopNextActor();
					state = GameState::RUNNING;
				}
				nextCommand = Command::NONE;
				timeSinceLastCommand = 0;
			}
			break;
	}
	timeSinceLastInput++;
	timeSinceLastCommand++;
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