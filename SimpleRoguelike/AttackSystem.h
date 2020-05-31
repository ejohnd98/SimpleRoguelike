#pragma once

#include "System.h"
#include "Commands.h"

class AttackSystem : public System
{
public:
	void Attack(Entity attacker, Entity target);
	bool WithinAttackRange(Entity attacker, Entity target);
	void DealDamage(Entity target, int damage);
};

