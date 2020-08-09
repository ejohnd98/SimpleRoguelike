#include <stdio.h>
#include <iostream>

#include "MapSystem.h"
#include "ECS.h"

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr<AnimationSystem> animationSystem;

void MapSystem::Init() {
}

void MapSystem::SetMap(Entity mapEntity) {
	assert(ecs->HasComponent<Map>(mapEntity));

	this->mapEntity = mapEntity;
	map = std::make_shared<Map>(ecs->GetComponent<Map>(mapEntity));
}

void MapSystem::PlaceEntity(Entity entity, Position newPos) {
	assert(ecs->HasComponent<Position>(entity));
	assert(ValidPosition(newPos));

	map->positionEntityMap[newPos] = entity;
	Position& pos = ecs->GetComponent<Position>(entity);
	Renderable& res = ecs->GetComponent<Renderable>(entity);
	res.position = newPos.ToFloat();
	pos = newPos;
}

void MapSystem::RemoveEntity(Entity entity) {
	assert(ecs->HasComponent<Position>(entity));

	Position& pos = ecs->GetComponent<Position>(entity);
	if (ValidPosition(pos) && map->positionEntityMap[pos] == entity) { 
		map->positionEntityMap.erase(pos);
	}
}

void MapSystem::MoveEntityRelative(Entity entity, Position offset) {
	MoveEntity(entity, ecs->GetComponent<Position>(entity) + offset);
}

void MapSystem::MoveEntity(Entity entity, Position newPos) {
	assert(ecs->HasComponent<Position>(entity));
	assert(ValidPosition(newPos));

	Position& pos = ecs->GetComponent<Position>(entity);
	if (GetEntityAt(pos) == entity) { //if entity in map, remove
		map->positionEntityMap.erase(pos);
	}

	map->positionEntityMap[newPos] = entity; //update map with new position
	animationSystem->AddMoveAnim(entity, pos.ToFloat(), newPos.ToFloat(), 0.2f);
	pos = newPos; //update entity's position

}

bool MapSystem::ValidPosition(Position pos) {
	return (pos.x >= 0 && pos.x < map->width && pos.y >= 0 && pos.y < map->height);
}

bool MapSystem::CanMoveTo(Position pos) { //returns true if position isn't a wall and contains no entity
	return (!map->GetCell(pos.x,pos.y)) && (GetEntityAt(pos) == NULL_ENTITY);
}

Entity MapSystem::GetEntityAt(Position pos) {
	if (map->positionEntityMap.find(pos) != map->positionEntityMap.end()) {
		return map->positionEntityMap[pos];
	}
	return NULL_ENTITY;
}

bool MapSystem::IsWall(Position pos) {
	return (map->GetCell(pos.x, pos.y) || !ValidPosition(pos));
}

void MapSystem::ClearVisible(bool includeVisited) {
	for (int y = 0; y < map->height; y++) {
		for (int x = 0; x < map->width; x++) {
			visible[y][x] = false;
			if (includeVisited) {
				known[y][x] = false;
			}
		}
	}
}

void MapSystem::SetVisible(int x, int y) {
	visible[y][x] = true;
}
void MapSystem::SetKnown(int x, int y) {
	known[y][x] = true;
}
bool MapSystem::IsVisible(int x, int y) {
	return visible[y][x];
}
bool MapSystem::IsKnown(int x, int y) {
	return known[y][x];
}