#pragma once
#include <queue>
#include "System.h"

class TurnSystem : public System
{
public:
	void Init();
	bool EntityCanAct();
	void PopNextActor();
	Entity PeekNextActor();
	bool PlayerActsNext();
	void DecreaseDebt(int amount);
	void AddDebt(Entity entity, ActionType action);

private:
	std::set<Entity> canAct;
	std::queue<Entity> actingOrder;
};

