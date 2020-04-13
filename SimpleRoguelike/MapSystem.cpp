#include <stdio.h>
#include <iostream>

#include "MapSystem.h"
#include "ECS.h"

extern ECS ecs;

void MapSystem::Init() {
}

void MapSystem::SetMap(Entity mapEntity) {
	assert(ecs.HasComponent<Map>(mapEntity));

	this->mapEntity = mapEntity;
	map = std::make_shared<Map>(ecs.GetComponent<Map>(mapEntity));
}

void MapSystem::PlaceEntity(Entity entity, Position newPos) {
	assert(ecs.HasComponent<Position>(entity));
	assert(ValidPosition(newPos));

	map->positionEntityMap[newPos] = entity;
	Position& pos = ecs.GetComponent<Position>(entity);
	pos = newPos;
}

void MapSystem::RemoveEntity(Entity entity) {
	assert(ecs.HasComponent<Position>(entity));

	Position& pos = ecs.GetComponent<Position>(entity);
	if (ValidPosition(pos) && map->positionEntityMap[pos] == entity) { 
		map->positionEntityMap.erase(pos);
	}
	pos = { -1,-1 };
}

void MapSystem::MoveEntity(Entity entity, Position newPos) {
	assert(ecs.HasComponent<Position>(entity));
	assert(ValidPosition(newPos));

	Position& pos = ecs.GetComponent<Position>(entity);
	if (map->positionEntityMap[pos] == entity) {
		map->positionEntityMap.erase(pos);
	}

	map->positionEntityMap[newPos] = entity;
	pos = newPos;

}

bool MapSystem::ValidPosition(Position pos) {
	return(pos.x >= 0 && pos.x <= map->width && pos.y >= 0 && pos.y <= map->height);
}