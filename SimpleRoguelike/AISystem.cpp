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
extern std::shared_ptr<InteractionHandler> interactionHandler;

void AISystem::DetermineAction() {
	currentEntity = *(entities.begin());
	AIControlled& ai = ecs->GetComponent<AIControlled>(currentEntity);
	Stats& stats = ecs->GetComponent<Stats>(currentEntity);

	Position& pos = ecs->GetComponent<Position>(currentEntity);
	Position nextPos;
	Position targetPos;
	if (ai.targetEntity != NULL_ENTITY && !ecs->HasComponent<Active>(ai.targetEntity)) {
		ai.targetEntity = NULL_ENTITY;
		ai.currentState = AIState::IDLE;
	}
	bool performedAction = false;

	while(!performedAction){
		switch (ai.currentState) {
		case AIState::IDLE:
			ai.targetEntity = GetTarget();
			if (ai.targetEntity != NULL_ENTITY) { //if target sighted, switch to attacking
				ai.currentState = AIState::ATTACKING;
			}
			else {
				performedAction = interactionHandler->PerformAction(currentEntity, {}, InteractType::WAIT);
			}		
			break;

		case AIState::WANDERING:
			ai.currentState = AIState::IDLE;
			break;

		case AIState::ATTACKING:
			targetPos = ecs->GetComponent<Position>(ai.targetEntity);
			if (fov->HasLineOfSight(pos, targetPos, stats.sight)) { //if target is visible, update last known position
				ai.lastTargetPos = targetPos;
			}
			
			if (damageSystem->WithinAttackRange(currentEntity, ai.targetEntity)) { //if target within range, attack
				performedAction = interactionHandler->PerformAction(currentEntity, targetPos, InteractType::ATTACK);
			}
			else { //otherwise move towards last known target position
				nextPos = pathfinding->GetPath(pos, ai.lastTargetPos, mapSystem->map);
				if (interactionHandler->PerformAction(currentEntity, nextPos, InteractType::MOVE)) {
					performedAction = true;
				}
				else {
					performedAction = interactionHandler->PerformAction(currentEntity, {}, InteractType::WAIT);
				}
			}
			break;

		case AIState::FLEEING:
			ai.currentState = AIState::IDLE;
			break;
		}
	}
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