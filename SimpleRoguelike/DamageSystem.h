#pragma once

#include "System.h"
#include "Commands.h"

class DamageSystem : public System
{
public:
	void Attack(Entity attacker, Entity target);
	bool WithinAttackRange(Entity attacker, Entity target);
	bool DealDamage(Entity target, int damage);
	void KillEntity(Entity entity);
};

