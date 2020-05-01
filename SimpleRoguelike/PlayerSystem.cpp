#include <stdio.h>
#include <iostream>

#include "PlayerSystem.h"
#include "ECS.h"

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr<MapSystem> mapSystem;
extern std::shared_ptr<AnimationSystem> animationSystem; //shouldn't need later (temp)
extern std::shared_ptr<TurnSystem> turnSystem;

void PlayerSystem::Init() {
}

bool PlayerSystem::DetermineAction(Command command) {
	switch (command) {
	case Command::MOVE_UP:
		return InteractWithCell(Position{ 0, -1 });
	case Command::MOVE_DOWN:
		return InteractWithCell(Position{ 0, 1 });
	case Command::MOVE_RIGHT:
		return InteractWithCell(Position{ 1, 0 });
	case Command::MOVE_LEFT:
		return InteractWithCell(Position{ -1, 0 });
	case Command::WAIT:
		return true;
	}
	return false;
}

Entity PlayerSystem::GetPlayerEntity() {
	if (entities.size() > 0) {
		return *(entities.begin());
	}
	return NULL_ENTITY;
}

bool PlayerSystem::InteractWithCell(Position offset) {
	Position pos = ecs->GetComponent<Position>(GetPlayerEntity());
	if (mapSystem->ValidPosition(pos + offset)) {
		//attempt in order:
		
		Entity other = mapSystem->GetEntityAt(pos + offset);
		if (other != NULL_ENTITY) {
			Sprite anim[] = { 112,113,114,115 };
			animationSystem->AddSpriteAnim(GetPlayerEntity(), anim, 4, 10);
			//act upon actor in position
			return true;
		}

		//move to position
		if (mapSystem->CanMoveTo(pos + offset)) {
			mapSystem->MoveEntityRelative(GetPlayerEntity(), offset);
			turnSystem->AddDebt(GetPlayerEntity(), 50); //temp hardcoded action cost
			return true;
		}
		//act upon prop in position
	}
	

	return false;
}

