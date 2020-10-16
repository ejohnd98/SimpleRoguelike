#pragma once
#include <stdio.h>
#include <iostream>
#include <queue>
#include <array>

#include "Constants.h"

class EntityManager
{
public:
	EntityManager();
	
	Entity CreateEntity();
	void DestroyEntity(Entity entity);
	void SetSignature(Entity entity, Signature signature);
	Signature GetSignature(Entity entity);
	int GetEntityCount();
private:
	std::queue<Entity> availableEntities;
	std::array<Signature, MAX_ENTITIES> signatures;

	int livingEntityCount;
};