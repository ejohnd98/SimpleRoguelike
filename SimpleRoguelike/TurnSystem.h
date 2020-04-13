#pragma once
#include <queue>
#include "System.h"

class TurnSystem : public System
{
public:
	void Init();
	bool EntityCanAct();
	Entity GetNextActor();
	void DecreaseDebt(int amount);
	void AddDebt(Entity entity, int amount);

private:
	std::set<Entity> canAct;
	std::queue<Entity> actingOrder;
};

