#include "PositionSystem.h"
#include "ECS.h"

extern ECS ecs;

void PositionSystem::Init() {
}

void PositionSystem::MoveTo(int x, int y) {
	for (auto const& entity : entities)
	{
		auto& position = ecs.GetComponent<Position>(entity);

		//move
		position.x = x;
		position.y = y;
	}
}