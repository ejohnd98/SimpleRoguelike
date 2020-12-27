#pragma once
#include "System.h"

class MapSystem : public System
{
public:
	void Init();
	void Clear();

	void SetMap(std::shared_ptr<Map> mapData);
	void PlaceEntity(Entity entity, Position pos);
	void RemoveEntity(Entity entity);
	void MoveEntityRelative(Entity entity, Position offset);
	void MoveEntity(Entity entity, Position pos);
	bool ValidPosition(Position pos);
	bool BlocksSight(Position pos);
	bool BlocksMovement(Position pos, bool ignoreActors = true);
	Entity GetEntityAt(Position pos);
	void ClearVisible(bool includeVisited = false);
	void SetVisible(int x, int y);
	void SetKnown(int x, int y);
	bool IsVisible(int x, int y);
	bool IsKnown(int x, int y);

	std::shared_ptr<Map> map;
	int currentMapId = -1;
	int totalMaps = 0;
private:
	bool visible[MAX_MAP_SIZE][MAX_MAP_SIZE];
	bool known[MAX_MAP_SIZE][MAX_MAP_SIZE];
};