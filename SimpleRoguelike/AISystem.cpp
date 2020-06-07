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
	Entity entity = *(entities.begin());
	AIControlled& ai = ecs->GetComponent<AIControlled>(entity);
	ActionType chosenAction = ActionType::NONE; //change to NONE once rest is implemented
	Entity targetEntity = playerSystem->GetPlayerEntity();

	Position& pos = ecs->GetComponent<Position>(entity);
	Position targetPos = ecs->GetComponent<Position>(targetEntity);
	Position nextPos;

	while(chosenAction==ActionType::NONE){
		switch (ai.currentState) {
		case AIState::IDLE:
			if (fov->HasLineOfSight(pos, targetPos)) { //if target sighted, switch to attacking
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
			if (damageSystem->WithinAttackRange(entity, targetEntity)) { //if target within range, attack
				damageSystem->Attack(entity, targetEntity);
				chosenAction = ActionType::ATTACK;
			}
			else { //otherwise move towards target
				nextPos = pathfinding->GetPath(pos, targetPos, mapSystem->map);
				if (mapSystem->CanMoveTo(nextPos)) { 
					mapSystem->MoveEntity(entity, nextPos);
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
	turnSystem->AddDebt(entity, chosenAction);
}