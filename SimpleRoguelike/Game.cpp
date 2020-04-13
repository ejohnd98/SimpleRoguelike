#include "Game.h"

//variables
int tickCounter;
ECS ecs;
std::shared_ptr<PositionSystem> positionSystem;
std::shared_ptr<TurnSystem> turnSystem;

Game::Game(){
	InitGame();
}
Game::~Game(){
	CloseGame();
}

bool Game::InitGame() {
	ecs.Init();

	//Register components
	ecs.RegisterComponent<Map>();
	ecs.RegisterComponent<Position>();
	ecs.RegisterComponent<Actor>();
	ecs.RegisterComponent<PlayerControlled>();

	//Register Position System
	positionSystem = ecs.RegisterSystem<PositionSystem>();
	Signature signature;
	signature.set(ecs.GetComponentType<Position>());
	ecs.SetSystemSignature<PositionSystem>(signature);

	//Register Turn System
	turnSystem = ecs.RegisterSystem<TurnSystem>();
	signature.reset();
	signature.set(ecs.GetComponentType<Actor>());
	ecs.SetSystemSignature<TurnSystem>(signature);

	//Debugging
	Entity test = ecs.CreateEntity();
	ecs.AddComponent(test, Actor{});
	test = ecs.CreateEntity();
	ecs.AddComponent(test, Actor{50});

	//Game setup
	tickCounter = 0;
	state = GameState::RUNNING;
	return true;
}
void Game::CloseGame() {

}

void Game::Advance() {
	switch (state) {
		case GameState::WAITING_INPUT:
			//if(valid input)
				//take action and indebt player
				//state = GameState::RUNNING;
			//else
				//do nothing
			break;
		case GameState::ANIMATING:
			//if(animations finished)
				//state = GameState::Running
			break;
		case GameState::RUNNING:
			if (turnSystem->EntityCanAct()) {
				Entity entity = turnSystem->GetNextActor();
				if (ecs.HasComponent<PlayerControlled>(entity)) {
					state = GameState::WAITING_INPUT;
				}
				else {
					std::cout << "Entity acting\n";
					turnSystem->AddDebt(entity, 100); //temp, simulate an action
				}
			}
			else {
				turnSystem->DecreaseDebt(10);
				tickCounter++;
				std::cout << tickCounter << "\n";
				if (tickCounter % 10 == 0) {
					state = GameState::ADVANCE_TURN;
				}
			}
			break;

			break;
		case GameState::ADVANCE_TURN:
			//increment turns counter
			//do turn effects like poison
			std::cout << "Advancing turn\n";
			state = GameState::RUNNING;
			break;
	}

}

void Game::GiveInput(Command command) {
	pendingCommands.push_back(command);
}