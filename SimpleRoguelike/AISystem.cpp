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

void AISystem::DetermineAction() {
	Entity entity = *(entities.begin());
	AIControlled& ai = ecs->GetComponent<AIControlled>(entity);
	ActionType chosenAction = ActionType::NONE; //change to NONE once rest is implemented

	Position& pos = ecs->GetComponent<Position>(entity);
	Position targetPos = ecs->GetComponent<Position>(playerSystem->GetPlayerEntity());
	Position nextPos;

	while(chosenAction==ActionType::NONE){
		switch (ai.currentState) {
		case AIState::IDLE:
			//if sighted target
			if (fov->HasLineOfSight(pos, targetPos)) {
				ai.currentState = AIState::ATTACKING;
				break;
			}
			else {
				chosenAction = ActionType::WAIT;
				break;
			}
			//else if [some timer]
				//ai.currentState = AIState::WANDERING;
				
			break;
		case AIState::WANDERING:
			//if sighted target
				ai.currentState = AIState::ATTACKING;
			//move randomly, or to random spot
			//chosenAction = ActionType::MOVE;
			break;
		case AIState::ATTACKING:
			//if [should flee]
				//ai.currentState = AIState::FLEEING;
			//else if target in range
				//chosenAction = ActionType::ATTACK;
				//Attack
			//else if last known position is recent
				//Move towards target
			nextPos = pathfinding->GetPath(pos, targetPos, mapSystem->map);
			//move to position
			if (mapSystem->CanMoveTo(nextPos)) {
				mapSystem->MoveEntity(entity, nextPos);
			}
			chosenAction = ActionType::MOVE;
			//else
				//ai.currentState = AIState::WANDERING;
			break;
		case AIState::FLEEING:
			//if [some timer]
				ai.currentState = AIState::IDLE;
			//else
				//move away from last known enemy pos
				//chosenAction = ActionType::MOVE;
			break;
		}
	}
	turnSystem->AddDebt(entity, chosenAction);
}