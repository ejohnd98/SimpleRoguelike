#pragma once

#include "Constants.h"
#include "Components.h"

class Pathfinding
{
public:
	static Position GetPath(Position a, Position b, std::shared_ptr<Map> map);
};

