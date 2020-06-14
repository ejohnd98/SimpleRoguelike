#include <stdio.h>
#include <iostream>

#include "AISystem.h"
#include "ECS.h"

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr <TurnSystem> turnSystem;
extern std::shared_ptr <AnimationSystem> animationSystem;
extern std::shared_ptr <MapSystem> mapSystem;
extern std::shared_ptr <Pathfinding> pathfinding;
extern std::shared_ptr <PlayerSystem> playerSystem;
extern std::shared_ptr<FieldOfView> fov;
extern std::shared_ptr<DamageSystem> damageSystem;

void AISystem::DetermineAction() {
	currentEntity = *(entities.begin());
	AIControlled& ai = ecs->GetComponent<AIControlled>(currentEntity);
	ActionType chosenAction = ActionType::NONE;

	Position& pos = ecs->GetComponent<Position>(currentEntity);
	Position nextPos;
	Position targetPos;
	if (ai.targetEntity != NULL_ENTITY && !ecs->HasComponent<Active>(ai.targetEntity)) {
		ai.targetEntity = NULL_ENTITY;
		ai.currentState = AIState::IDLE;
	}

	while(chosenAction==ActionType::NONE){
		switch (ai.currentState) {
		case AIState::IDLE:
			ai.targetEntity = GetTarget();
			if (ai.targetEntity != NULL_ENTITY) { //if target sighted, switch to attacking
				ai.currentState = AIState::ATTACKING;
			}
			else {
				chosenAction = ActionType::WAIT;
			}		
			break;

		case AIState::WANDERING:
			ai.currentState = AIState::IDLE;
			break;

		case AIState::ATTACKING:
			targetPos = ecs->GetComponent<Position>(ai.targetEntity);
			if (fov->HasLineOfSight(pos, targetPos)) { //if target is visible, update last known position
				ai.lastTargetPos = targetPos;
			}
			
			if (damageSystem->WithinAttackRange(currentEntity, ai.targetEntity)) { //if target within range, attack
				chosenAction = ActionType::ATTACK;
				damageSystem->Attack(currentEntity, ai.targetEntity);
			}
			else { //otherwise move towards last known target position
				nextPos = pathfinding->GetPath(pos, ai.lastTargetPos, mapSystem->map);
				if (mapSystem->CanMoveTo(nextPos)) { 
					mapSystem->MoveEntity(currentEntity, nextPos);
					chosenAction = ActionType::MOVE;
				}
				else {
					chosenAction = ActionType::WAIT;
				}
			}
			break;

		case AIState::FLEEING:
			ai.currentState = AIState::IDLE;
			break;
		}
	}
	turnSystem->AddDebt(currentEntity, chosenAction);
}

Entity AISystem::GetTarget() {
	auto visibleEntities = fov->GetVisibleEntities(currentEntity);
	for (Entity e : visibleEntities) {
		if (ecs->HasComponent<PlayerControlled>(e)) {
			return e;
		}
	}
	return NULL_ENTITY;
}