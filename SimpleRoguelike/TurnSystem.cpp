#include <stdio.h>
#include <iostream>
#include <unordered_map>

#include "TurnSystem.h"
#include "ECS.h"

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr <MapSystem> mapSystem;

std::unordered_map<InteractType, int> interactionDebtMap{
	{InteractType::WAIT, 100},
	{InteractType::MOVE, 100},
	{InteractType::ATTACK, 100},
	{InteractType::PROP, 100},
	{InteractType::CLOSE, 100},
	{InteractType::PICK_UP, 100},
};

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
	Entity next = actingOrder.front();

	while (EntityCanAct()) {
		next = actingOrder.front();
		if (!ecs->HasComponent<Active>(next)) {
			PopNextActor();
		}
		else {
			return next;
		}
	}
	return NULL_ENTITY;
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
		Renderable& r = ecs->GetComponent<Renderable>(entity);
		if (r.mapId != mapSystem->currentMapId) {
			continue;
		}
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

void TurnSystem::AddDebt(Entity entity, InteractType action) {
	Actor& actor = ecs->GetComponent<Actor>(entity);
	Stats& stats = ecs->GetComponent<Stats>(entity);
	float debtMod = (10.0 / (float)stats.spd);
	int debt = (int)(interactionDebtMap.at(action) * debtMod);

	actor.actionDebt += debt;
}