#include <stdio.h>
#include <iostream>

#include "PlayerSystem.h"
#include "ECS.h"

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr<MapSystem> mapSystem;
extern std::shared_ptr<AnimationSystem> animationSystem; //shouldn't need later (temp)
extern std::shared_ptr<TurnSystem> turnSystem;
extern std::shared_ptr<DamageSystem> damageSystem;
extern std::shared_ptr<InteractionSystem> interactionSystem;

void PlayerSystem::Init() {
}

bool PlayerSystem::DetermineAction(Command command) {
	bool performedAction = false;
	switch (command) {
	case Command::MOVE_UP:
		performedAction = InteractWithCell(Position{ 0, -1 });
		break;
	case Command::MOVE_DOWN:
		performedAction = InteractWithCell(Position{ 0, 1 });
		break;
	case Command::MOVE_RIGHT:
		performedAction = InteractWithCell(Position{ 1, 0 });
		break;
	case Command::MOVE_LEFT:
		performedAction = InteractWithCell(Position{ -1, 0 });
		break;
	case Command::WAIT:
		performedAction = interactionSystem->PerformAction(GetPlayerEntity(), {}, InteractType::WAIT);
		break;
	}
	return performedAction;
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
		if (interactionSystem->PerformAction(GetPlayerEntity(), pos + offset, InteractType::MOVE)) {
			return true;
		}
		if (interactionSystem->PerformAction(GetPlayerEntity(), pos + offset, InteractType::OPEN)) {
			return true;
		}
		if (interactionSystem->PerformAction(GetPlayerEntity(), pos + offset, InteractType::ATTACK)) {
			return true;
		}
	}
	return false;
}

