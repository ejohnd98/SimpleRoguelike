#include <stdio.h>
#include <iostream>

#include "TurnSystem.h"
#include "ECS.h"

extern ECS ecs;

void TurnSystem::Init() {
}

bool TurnSystem::EntityCanAct() {
	return !canAct.empty();
}

Entity TurnSystem::GetNextActor() {
	if (EntityCanAct()) {
		Entity next = actingOrder.front();
		actingOrder.pop();
		canAct.erase(next);
		return next;
	}
	else {
		return NULL_ENTITY;
	}
}

void TurnSystem::DecreaseDebt(int amount) {
	for (auto const& entity : entities) //iterate through all actors
	{
		Actor& actor = ecs.GetComponent<Actor>(entity);

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

void TurnSystem::AddDebt(Entity entity, int amount) {
	Actor& actor = ecs.GetComponent<Actor>(entity);
	actor.actionDebt += amount;
}