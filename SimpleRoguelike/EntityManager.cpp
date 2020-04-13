#include <assert.h>

#include "EntityManager.h"

EntityManager::EntityManager() {
	//populate availableEntities with all possible entities
	for (Entity e = 1; e <= MAX_ENTITIES; e++) {
		availableEntities.push(e);
	}
	livingEntityCount = 0;
}

Entity EntityManager::CreateEntity() {
	assert(livingEntityCount < MAX_ENTITIES);

	Entity id = availableEntities.front();
	availableEntities.pop();
	livingEntityCount++;

	return id;
}

void EntityManager::DestroyEntity(Entity entity) {
	assert(entity <= MAX_ENTITIES);

	signatures[entity].reset(); //reset bitfield signature
	availableEntities.push(entity); //make id available again
	livingEntityCount--;
}

void EntityManager::SetSignature(Entity entity, Signature signature) {
	assert(entity <= MAX_ENTITIES);

	signatures[entity] = signature;
}

Signature EntityManager::GetSignature(Entity entity) {
	assert(entity <= MAX_ENTITIES);

	return signatures[entity];
}