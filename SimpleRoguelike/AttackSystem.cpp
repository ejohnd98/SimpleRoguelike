#include <stdio.h>
#include <iostream>

#include "AttackSystem.h"
#include "ECS.h"

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr <AnimationSystem> animationSystem;
extern std::shared_ptr <MapSystem> mapSystem;

void AttackSystem::Attack(Entity attacker, Entity target) {
	Stats& attackerStats = ecs->GetComponent<Stats>(attacker);
	Stats& targetStats = ecs->GetComponent<Stats>(target);

	Sprite anim[] = { 112,113,114,115 };
	animationSystem->AddSpriteAnim(attacker, anim, 4, 10);

	int damage = std::max( attackerStats.strength - targetStats.defense , 0);
	std::cout << "Entity " << attacker << " dealt " << damage << " damage to Entity " << target << "\n";
	DealDamage(target, damage);
}

void AttackSystem::DealDamage(Entity target, int damage) {
	Stats& targetStats = ecs->GetComponent<Stats>(target);
	targetStats.health -= damage;

	if (targetStats.health <= 0) {
		std::cout << "Entity " << target << " has been killed\n";
	}
}

bool AttackSystem::WithinAttackRange(Entity attacker, Entity target) {
	Position dist = ecs->GetComponent<Position>(attacker) - ecs->GetComponent<Position>(target);
	return dist.Magnitude() <= 1;
}

