#include <SDL.h>
#include <stdio.h>
#include <iostream>

#include "ECS.h"
#include "Game.h"
#include "Commands.h"
#include "Constants.h"

//Screen constants
const int SCREEN_FPS = 60;
const int UPDATES_PER_SECOND = 60;
const int MS_PER_FRAME = 1000 / SCREEN_FPS;
const int MS_PER_UPDATE = 1000 / UPDATES_PER_SECOND;

//Functions
bool Initialize();
void Terminate();
Command InputToCommand(SDL_Event* e);

//Variables
std::shared_ptr <ECS> ecs;
std::shared_ptr<RendererSystem> rendererSystem;
std::shared_ptr<TurnSystem> turnSystem;
std::shared_ptr<MapSystem> mapSystem;
std::shared_ptr<PlayerSystem> playerSystem;
std::shared_ptr<AnimationSystem> animationSystem;
std::shared_ptr<Game> game;

bool Initialize()
{
	bool success = true;
	ecs = std::make_unique<ECS>();
	ecs->Init();
	
	//Register components
	ecs->RegisterComponent<Map>();
	ecs->RegisterComponent<Position>();
	ecs->RegisterComponent<Actor>();
	ecs->RegisterComponent<PlayerControlled>();
	ecs->RegisterComponent<Renderable>();
	ecs->RegisterComponent<AnimIdle>();
	ecs->RegisterComponent<AnimSprite>();
	ecs->RegisterComponent<AnimMove>();

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
	ecs->SetSystemSignature<TurnSystem>(signature);

	//Register Map System (Handles all interactions with Map)
	mapSystem = ecs->RegisterSystem<MapSystem>();
	signature.reset();
	signature.set(ecs->GetComponentType<Map>());
	ecs->SetSystemSignature<MapSystem>(signature);

	//Register Player System (Interface between incoming commands and player controlled character)
	playerSystem = ecs->RegisterSystem<PlayerSystem>();
	signature.reset();
	signature.set(ecs->GetComponentType<PlayerControlled>());
	ecs->SetSystemSignature<PlayerSystem>(signature);

	//Register Animation System (Interface for adding animations to entities)
	animationSystem = ecs->RegisterSystem<AnimationSystem>();
	signature.reset();
	ecs->SetSystemSignature<PlayerSystem>(signature);

	//Create Game (Contains main game loop)
	game = std::make_shared<Game>();

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
					case SDL_KEYDOWN:
						//game->GiveInput(InputToCommand(&event));
						break;
					case SDL_QUIT:
						quit = true;
						break;
					default:
						break;
						//game->ClearHeldInput();
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
				game->Advance();
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