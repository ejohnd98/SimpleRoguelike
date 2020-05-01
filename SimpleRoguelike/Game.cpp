#include <stdio.h>
#include <iostream>

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
extern std::shared_ptr<RendererSystem> rendererSystem;
extern std::shared_ptr<AnimationSystem> animationSystem;

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
	ecs->AddComponent(player, Renderable{0});
	Sprite anim[] = { 32,33 };
	animationSystem->AddIdleAnim(player, anim, 2, 30);

	Entity enemy = ecs->CreateEntity();
	ecs->AddComponent(enemy, Actor{0});
	ecs->AddComponent(enemy, Position{});
	ecs->AddComponent(enemy, Renderable{0});
	Sprite anim2[] = { 37,38 };
	animationSystem->AddIdleAnim(enemy, anim2, 2, 60);
	
	mapSystem->PlaceEntity(player, { 1,1 });
	mapSystem->PlaceEntity(enemy, { 3,1 });

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
				state = GameState::RUNNING;
			}
		case GameState::RUNNING:
			while (!turnSystem->EntityCanAct() && tickLimit++ < 50) {
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
				Entity entity = turnSystem->PeekNextActor();
				turnSystem->PopNextActor();
				std::cout << "Entity acting\n";
				//Add some sort of component to entity
				//call system to run AI for the entity
				turnSystem->AddDebt(entity, 100); //temp, simulate an action
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
					std::cout << "Player acting\n";
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