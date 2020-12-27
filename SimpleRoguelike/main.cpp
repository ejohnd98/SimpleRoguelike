#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include <ctime>

#include "ECS.h"
#include "Game.h"
#include "Commands.h"
#include "Constants.h"
#include "EntityFactory.h"
#include "RandomUtil.h"
#include "DungeonGenerator.h"

//Screen constants
const int SCREEN_FPS = 60;
const int UPDATES_PER_SECOND = 60;
const int MS_PER_FRAME = 1000 / SCREEN_FPS;
const int MS_PER_UPDATE = 1000 / UPDATES_PER_SECOND;

//Functions
bool Initialize();
void Terminate();

//Variables
std::shared_ptr <ECS> ecs;
std::shared_ptr<RendererSystem> rendererSystem;
std::shared_ptr<TurnSystem> turnSystem;
std::shared_ptr<MapSystem> mapSystem;
std::shared_ptr <DungeonSystem> dungeonSystem;
std::shared_ptr<PlayerSystem> playerSystem;
std::shared_ptr<AISystem> aiSystem;
std::shared_ptr<AnimationSystem> animationSystem;
std::shared_ptr<DamageSystem> damageSystem;
std::shared_ptr<LogSystem> logSystem;
std::shared_ptr<MemorySystem> memorySystem;

std::shared_ptr<Game> game;
std::shared_ptr<Pathfinding> pathfinding;
std::shared_ptr<FieldOfView> fov;
std::shared_ptr<InteractionHandler> interactionHandler;
std::shared_ptr<EntityFactory> entityFactory;
std::shared_ptr<RandomUtil> randomUtil;

int ticksStart = -1;
bool shownMapGenTime = false;

bool Initialize()
{
	bool success = true;
	ecs = std::make_unique<ECS>();
	ecs->Init();
	
	//Register components
	ecs->RegisterComponent<Map>();
	ecs->RegisterComponent<Dungeon>();
	ecs->RegisterComponent<Position>();
	ecs->RegisterComponent<Actor>();
	ecs->RegisterComponent<PlayerControlled>();
	ecs->RegisterComponent<ActiveAIEntity>();
	ecs->RegisterComponent<AIControlled>();
	ecs->RegisterComponent<Openable>();
	ecs->RegisterComponent<Renderable>();
	ecs->RegisterComponent<AnimIdle>();
	ecs->RegisterComponent<AnimSprite>();
	ecs->RegisterComponent<AnimMove>();
	ecs->RegisterComponent<DeleteAfterAnim>();
	ecs->RegisterComponent<Info>();
	ecs->RegisterComponent<PropInfo>();
	ecs->RegisterComponent<Stats>();
	ecs->RegisterComponent<Active>();

	//Create random Utils;
	randomUtil = std::make_shared<RandomUtil>((unsigned int)std::time(0));
	//randomUtil = std::make_shared<RandomUtil>(1222);
	printf("game seed is %d\n", (unsigned int)std::time(0));

	//Register Renderer System (Interfaces with SDL to render game)
	rendererSystem = ecs->RegisterSystem<RendererSystem>();
	Signature signature;
	signature.set(ecs->GetComponentType<Renderable>());
	ecs->SetSystemSignature<RendererSystem>(signature);
	rendererSystem->Init();

	//Register Turn System (Handles turn order and action debts)
	turnSystem = ecs->RegisterSystem<TurnSystem>();
	signature.reset();
	signature.set(ecs->GetComponentType<Actor>());
	signature.set(ecs->GetComponentType<Active>());
	ecs->SetSystemSignature<TurnSystem>(signature);

	//Register Map System (Handles all interactions with Map)
	mapSystem = ecs->RegisterSystem<MapSystem>();
	signature.reset();
	signature.set(ecs->GetComponentType<Map>());
	ecs->SetSystemSignature<MapSystem>(signature);

	dungeonSystem = ecs->RegisterSystem<DungeonSystem>();
	signature.reset();
	ecs->SetSystemSignature<DungeonSystem>(signature);

	//Register Player System (Interface between incoming commands and player controlled character)
	playerSystem = ecs->RegisterSystem<PlayerSystem>();
	signature.reset();
	signature.set(ecs->GetComponentType<PlayerControlled>());
	signature.set(ecs->GetComponentType<Active>());
	ecs->SetSystemSignature<PlayerSystem>(signature);

	//Register AI System (Takes in active AI entity and decides their actions)
	aiSystem = ecs->RegisterSystem<AISystem>();
	signature.reset();
	signature.set(ecs->GetComponentType<ActiveAIEntity>());
	ecs->SetSystemSignature<AISystem>(signature);

	//Register Animation System (Interface for adding animations to entities)
	animationSystem = ecs->RegisterSystem<AnimationSystem>();
	signature.reset();
	ecs->SetSystemSignature<AnimationSystem>(signature);

	//Register Attack System (Performs damage calculations and damage dealing)
	damageSystem = ecs->RegisterSystem<DamageSystem>();
	signature.reset();
	ecs->SetSystemSignature<DamageSystem>(signature);

	//Register Attack System (Performs damage calculations and damage dealing)
	logSystem = ecs->RegisterSystem<LogSystem>();
	signature.reset();
	ecs->SetSystemSignature<LogSystem>(signature);

	//Register Memory System
	memorySystem = ecs->RegisterSystem<MemorySystem>();
	signature.reset();
	ecs->SetSystemSignature<MemorySystem>(signature);

	//Register Interaction System
	interactionHandler = std::make_shared<InteractionHandler>();

	//Create Pathfinding
	pathfinding = std::make_shared<Pathfinding>();

	//Create FOV
	fov = std::make_shared<FieldOfView>();

	//Create Game (Contains main game loop)
	game = std::make_shared<Game>();

	//Create Entity Factory
	entityFactory = std::make_shared<EntityFactory>();

	

	if (!game) {
		printf("ERROR: Failed to create game!\n");
	}
	return success;
}

void Terminate()
{
	rendererSystem->Close();
}

Command InputToCommand() { //hardcoded inputs currently
	void SDL_PumpEvents(void);
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_GetScancodeFromKey(SDLK_UP)]) {
		return Command::MOVE_UP;
	}
	if (keys[SDL_GetScancodeFromKey(SDLK_DOWN)]) {
		return Command::MOVE_DOWN;
	}
	if (keys[SDL_GetScancodeFromKey(SDLK_RIGHT)]) {
		return Command::MOVE_RIGHT;
	}
	if (keys[SDL_GetScancodeFromKey(SDLK_LEFT)]) {
		return Command::MOVE_LEFT;
	}
	if (keys[SDL_GetScancodeFromKey(SDLK_SPACE)]) {
		return Command::WAIT;
	}
	return Command::NONE;
}

int main(int argc, char* args[]){

	//Start up SDL and create window
	if (Initialize()){
		bool quit = false; //Main loop flag

		Uint32 currentTime = SDL_GetTicks();
		Uint32 nextGameUpdateTime = SDL_GetTicks();
		Uint32 nextRenderTime = SDL_GetTicks() + MS_PER_FRAME;

		SDL_Event event; //Event handler

		while (!quit) {
			//Handle events on queue
			while (SDL_PollEvent(&event) != 0){
				switch (event.type) {
					case SDL_QUIT:
						quit = true;
						break;
					default:
						break;
				}
			}
			Command input = InputToCommand();
			if (input != Command::NONE) {
				game->GiveInput(input);
			}else {
				game->ClearHeldInput();
			}

			currentTime = SDL_GetTicks();
			if (currentTime >= nextGameUpdateTime) {
				nextGameUpdateTime += MS_PER_UPDATE;
				int diff = ((int)nextGameUpdateTime) - ((int)currentTime);
				//std::cout << "Advancing loop at: " << currentTime << " Next: " << nextGameUpdateTime << " Diff: " << diff << "\n";
				if (ticksStart == -1) {
					ticksStart = currentTime;
				}

				game->Advance();
				while (game->StillProcessing()) {
					game->Advance(true);
				}
				if (!shownMapGenTime && game->dunGen->IsFinished()) {
					float timeTaken = (currentTime - ticksStart) *0.001f;
					printf("mapGen time: %f\n", timeTaken);
					if (DEBUG_MAP_GEN) {
						ticksStart = currentTime;
						shownMapGenTime = false;
					}
					else {
						shownMapGenTime = true;
					}
				}
				
			}
			if (currentTime >= nextRenderTime) { //quite flawed, as there is no need to "catch up" on missed frames (unlike game updates), but will correct later.
				nextRenderTime += MS_PER_FRAME;
				rendererSystem->Render();
			}
		}
	}
	Terminate(); //Free resources and close SDL
	return 0;
}