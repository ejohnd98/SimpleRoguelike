#include <stdio.h>
#include <iostream>

#include "AISystem.h"
#include "ECS.h"

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr <TurnSystem> turnSystem;
extern std::shared_ptr <AnimationSystem> animationSystem;

void AISystem::DetermineAction() {
	Entity entity = *(entities.begin());
	AIControlled& ai = ecs->GetComponent<AIControlled>(entity);
	Sprite anim[] = { 112,113,114,115 };
	animationSystem->AddSpriteAnim(entity, anim, 4, 10);
	ActionType chosenAction = ActionType::NONE;

	//while(chosenAction!=ActionType::None){
		switch (ai.currentState) {
		case AIState::IDLE:
			//if sighted target
				//ai.currentState = AIState::ATTACKING;
			//else if [some timer]
				//ai.currentState = AIState::WANDERING;
			//else
				//do nothing
				//chosenAction = ActionType::WAIT;
			break;
		case AIState::WANDERING:
			//if sighted target
				//ai.currentState = AIState::ATTACKING;
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
				//chosenAction = ActionType::MOVE;
			//else
				//ai.currentState = AIState::WANDERING;
			break;
		case AIState::FLEEING:
			//if [some timer]
				//ai.currentState = AIState::IDLE;
			//else
				//move away from last known enemy pos
				//chosenAction = ActionType::MOVE;
			break;
		}
	//}
	//Create function in turn system to calculate debt
	turnSystem->AddDebt(entity, 200);

}