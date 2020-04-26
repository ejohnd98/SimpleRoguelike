#pragma once
#include "System.h"

class MapSystem : public System
{
public:
	void Init();
	void SetMap(Entity mapEntity);
	void PlaceEntity(Entity entity, Position pos);
	void RemoveEntity(Entity entity);
	void MoveEntity(Entity entity, Position pos);
	bool ValidPosition(Position pos);
	bool CanMoveTo(Position pos);
	Entity GetEntityAt(Position pos);

	Entity mapEntity;
	std::shared_ptr<Map> map;
};