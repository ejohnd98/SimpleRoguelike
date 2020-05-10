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
	ActionType chosenAction = ActionType::NONE;
	switch (command) {
	case Command::MOVE_UP:
		chosenAction = InteractWithCell(Position{ 0, -1 });
		break;
	case Command::MOVE_DOWN:
		chosenAction = InteractWithCell(Position{ 0, 1 });
		break;
	case Command::MOVE_RIGHT:
		chosenAction = InteractWithCell(Position{ 1, 0 });
		break;
	case Command::MOVE_LEFT:
		chosenAction = InteractWithCell(Position{ -1, 0 });
		break;
	case Command::WAIT:
		chosenAction = ActionType::WAIT;
		break;
	}
	//Create function in turn system to calculate debt
	if (chosenAction != ActionType::NONE) {
		turnSystem->AddDebt(GetPlayerEntity(), chosenAction);
	}
	return (chosenAction != ActionType::NONE);
}

Entity PlayerSystem::GetPlayerEntity() {
	if (entities.size() > 0) {
		return *(entities.begin());
	}
	return NULL_ENTITY;
}

ActionType PlayerSystem::InteractWithCell(Position offset) {
	Position pos = ecs->GetComponent<Position>(GetPlayerEntity());
	if (mapSystem->ValidPosition(pos + offset)) {
		//attempt in order:
		
		Entity other = mapSystem->GetEntityAt(pos + offset);
		if (other != NULL_ENTITY) {
			Sprite anim[] = { 112,113,114,115 };
			animationSystem->AddSpriteAnim(GetPlayerEntity(), anim, 4, 10);
			//act upon actor in position
			return ActionType::ATTACK;
		}

		//move to position
		if (mapSystem->CanMoveTo(pos + offset)) {
			mapSystem->MoveEntityRelative(GetPlayerEntity(), offset);
			return ActionType::MOVE;
		}
		//act upon prop in position
	}
	return ActionType::NONE;
}

