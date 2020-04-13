#pragma once
#include "System.h"

class PositionSystem : public System
{
public:
	void Init();

	void MoveTo(int x, int y);
};

