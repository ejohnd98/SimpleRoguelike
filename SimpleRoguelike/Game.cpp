#include "Game.h"

//variables
int tickCounter;
ECS ecs;
std::shared_ptr<PositionSystem> positionSystem;
std::shared_ptr<TurnSystem> turnSystem;
std::shared_ptr<MapSystem> mapSystem;

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

	//Register Map System
	mapSystem = ecs.RegisterSystem<MapSystem>();
	signature.reset();
	signature.set(ecs.GetComponentType<Map>());
	ecs.SetSystemSignature<MapSystem>(signature);

	//hardcoded first map
	Entity mapEntity = ecs.CreateEntity();
	ecs.AddComponent(mapEntity, Map{});
	mapSystem->SetMap(mapEntity);

	Entity player = ecs.CreateEntity();
	ecs.AddComponent(player, Actor{0});
	ecs.AddComponent(player, PlayerControlled{});
	ecs.AddComponent(player, Position{-44,-414});
	mapSystem->PlaceEntity(player, { 1,1 });

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
			if (!pendingCommands.empty()) {
				Command command = pendingCommands.front();
				pendingCommands.pop();
				Entity entity = turnSystem->PeekNextActor();
				//if valid move:
					std::cout << "Player acting\n";
					turnSystem->AddDebt(entity, 50); //temp, simulate an action
					turnSystem->PopNextActor();
					state = GameState::RUNNING;
			}
			break;
		case GameState::ANIMATING:
			//if(animations finished)
				//state = GameState::Running
			break;
		case GameState::RUNNING:
			if (turnSystem->EntityCanAct()) {
				if (turnSystem->PlayerActsNext()) {
					state = GameState::WAITING_INPUT;
				}
				else {
					Entity entity = turnSystem->PeekNextActor();
					turnSystem->PopNextActor();
					std::cout << "Entity acting\n";
					//Add some sort of component to entity
					//call system to run AI for the entity
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
		case GameState::ADVANCE_TURN:
			//increment turns counter
			//do turn effects like poison
			std::cout << "Advancing turn\n";
			state = GameState::RUNNING;
			break;
	}
}

void Game::GiveInput(Command command) {
	if (AcceptingInput()) { //use switch statement later to still accept input for things like menu that don't depend on game state
		pendingCommands.push(command);
	}
}

bool Game::AcceptingInput() {
	return state == GameState::WAITING_INPUT;
}