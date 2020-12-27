#include <stdio.h>
#include <iostream>

#include "MapSystem.h"
#include "ECS.h"

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr<AnimationSystem> animationSystem;

void MapSystem::Init() {
}

void MapSystem::Clear() {
	for (std::pair<Position, Entity> element : map->positionEntityMap) {
		ecs->DestroyEntity(element.second);
		map->positionEntityMap.erase(element.first);
	}
	map->width = 0;
	map->height = 0;
	map->entrance = { -1,-1 };
	map->exit = { -1,-1 };
	map = nullptr;
	
}

void MapSystem::SetMap(std::shared_ptr<Map> mapData) {
	map = mapData;
	if (mapData->mapId < 0) {
		totalMaps++;
		mapData->mapId = totalMaps;
	}
	currentMapId = mapData->mapId;
}

void MapSystem::PlaceEntity(Entity entity, Position newPos) {
	assert(ecs->HasComponent<Position>(entity));
	assert(ValidPosition(newPos));

	map->positionEntityMap[newPos] = entity;
	Position& pos = ecs->GetComponent<Position>(entity);
	Renderable& res = ecs->GetComponent<Renderable>(entity);
	res.position = newPos.ToFloat();
	res.mapId = currentMapId;
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
	pos = newPos; //update entity's position

}

bool MapSystem::ValidPosition(Position pos) {
	return (pos.x >= 0 && pos.x < map->width && pos.y >= 0 && pos.y < map->height);
}

Entity MapSystem::GetEntityAt(Position pos) {
	if (map->positionEntityMap.find(pos) != map->positionEntityMap.end()) {
		return map->positionEntityMap[pos];
	}
	return NULL_ENTITY;
}

bool MapSystem::BlocksSight(Position pos) {
	if (!ValidPosition(pos) || map->GetCell(pos.x, pos.y)) {
		return true;
	}
	
	if (map->positionEntityMap.find(pos) != map->positionEntityMap.end()) {
		Entity entity = map->positionEntityMap.at(pos);
		if (ecs->HasComponent<Openable>(entity) && !ecs->GetComponent<Openable>(entity).isOpen) {
			return true;
		}
	}
	return false;
}

bool MapSystem::BlocksMovement(Position pos, bool ignoreActors) {
	if (!ValidPosition(pos) || map->GetCell(pos.x, pos.y)) {
		return true;
	}

	if (map->positionEntityMap.find(pos) != map->positionEntityMap.end()) {
		Entity entity = map->positionEntityMap.at(pos);
		if (ecs->HasComponent<PropInfo>(entity)) {
			auto& info = ecs->GetComponent<PropInfo>(entity);
			switch (info.function) {
			case PropFunction::DOOR: {
				if (ecs->HasComponent<Openable>(entity) && !ecs->GetComponent<Openable>(entity).isOpen) {
					return true;
				}
				break;
			}
			default:
				return true;
			};
		}
		if (!ignoreActors && ecs->HasComponent<Actor>(entity)) {
			return true;
		}
	}
	return false;
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