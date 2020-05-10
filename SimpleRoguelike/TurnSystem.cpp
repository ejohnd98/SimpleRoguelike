#include <stdio.h>
#include <iostream>

#include "TurnSystem.h"
#include "ECS.h"

extern std::shared_ptr <ECS> ecs;

void TurnSystem::Init() {
}

bool TurnSystem::EntityCanAct() {
	return !canAct.empty();
}

void TurnSystem::PopNextActor() {
	if (EntityCanAct()) {
		Entity next = actingOrder.front();
		actingOrder.pop();
		canAct.erase(next);
	}
}

Entity TurnSystem::PeekNextActor() {
	if (EntityCanAct()) {
		return actingOrder.front();
	}
	else {
		return NULL_ENTITY;
	}
}

bool TurnSystem::PlayerActsNext() {
	if (EntityCanAct()) {
		Entity next = actingOrder.front();
		return ecs->HasComponent<PlayerControlled>(next);
	}
	return false;
}

void TurnSystem::DecreaseDebt(int amount) {
	for (auto const& entity : entities) //iterate through all actors
	{
		Actor& actor = ecs->GetComponent<Actor>(entity);

		if (actor.actionDebt > 0) { //if actor has debt, decrease by given amount
			actor.actionDebt -= amount;
		}
		if (actor.actionDebt <= 0) { //if actor has no debt
			actor.actionDebt = 0; //clamp debt to zero
			if (canAct.find(entity) == canAct.end()) { //add to set and queue if not in already
				canAct.insert(entity);
				actingOrder.push(entity);
			}
		}
	}
}

void TurnSystem::AddDebt(Entity entity, ActionType action) {
	Actor& actor = ecs->GetComponent<Actor>(entity);
	int debt = 0;
	switch (action) {
		case ActionType::MOVE:
			debt = 100;
			break;
		case ActionType::ATTACK:
			debt = 50;
			break;
		case ActionType::WAIT:
			debt = 100;
			break;
	}
	actor.actionDebt += debt;
}