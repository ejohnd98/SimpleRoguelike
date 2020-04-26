#include <stdio.h>
#include <iostream>

#include "PlayerSystem.h"
#include "ECS.h"

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr<MapSystem> mapSystem;

void PlayerSystem::Init() {
}

bool PlayerSystem::DetermineAction(Command command) {
	Position pos = ecs->GetComponent<Position>(GetPlayerEntity());
	switch (command) {
	case Command::MOVE_UP:
		return InteractWithCell(pos + Position{ 0, -1 });
	case Command::MOVE_DOWN:
		return InteractWithCell(pos + Position{ 0, 1 });
	case Command::MOVE_RIGHT:
		return InteractWithCell(pos + Position{ 1, 0 });
	case Command::MOVE_LEFT:
		return InteractWithCell(pos + Position{ -1, 0 });
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

bool PlayerSystem::InteractWithCell(Position pos) {
	
	if (mapSystem->ValidPosition(pos)) {
		//attempt in order:
		
		Entity other = mapSystem->GetEntityAt(pos);
		if (other != NULL_ENTITY) {
			//act upon actor in position
			return true;
		}

		//move to position
		if (mapSystem->CanMoveTo(pos)) {
			mapSystem->MoveEntity(GetPlayerEntity(), pos);
			return true;
		}
		//act upon prop in position
	}
	

	return false;
}

