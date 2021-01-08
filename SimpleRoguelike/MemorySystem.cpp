#include <stdio.h>
#include <iostream>

#include "MemorySystem.h"
#include "ECS.h"

extern std::shared_ptr <ECS> ecs;

void MemorySystem::CleanUpEntities() {
	int count = 0;
	int removed = 0;
	for (auto const& entity : entities) //iterate through all actors
	{
		count++;
		if (!ecs->HasComponent<Active>(entity) && !ecs->HasComponent<PlayerControlled>(entity)) {
			ecs->DestroyEntity(entity);
			removed++;
		}
	}
}