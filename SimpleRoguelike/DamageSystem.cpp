#include <stdio.h>
#include <iostream>

#include "DamageSystem.h"
#include "ECS.h"

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr <AnimationSystem> animationSystem;
extern std::shared_ptr <MapSystem> mapSystem;

void DamageSystem::Attack(Entity attacker, Entity target) {
	Stats& attackerStats = ecs->GetComponent<Stats>(attacker);
	Stats& targetStats = ecs->GetComponent<Stats>(target);

	Sprite anim[] = { 112,113,114,115,116 };
	int lengths[] = { 5, 2, 2, 2, 5 };
	FloatPosition pos = ecs->GetComponent<Position>(target).ToFloat();

	animationSystem->AddSpriteAnim(pos, anim, (Tileset)"16x16_tileset", 5, lengths);

	int damage = std::max( attackerStats.strength - targetStats.defense , 0);
	std::cout << "Entity " << attacker << " dealt " << damage << " damage to Entity " << target << "\n";
	DealDamage(target, damage);
}

void DamageSystem::DealDamage(Entity target, int damage) {
	Stats& targetStats = ecs->GetComponent<Stats>(target);
	targetStats.health -= damage;

	if (targetStats.health <= 0) {
		std::cout << "Entity " << target << " has been killed\n";
		KillEntity(target);
	}
}

bool DamageSystem::WithinAttackRange(Entity attacker, Entity target) {
	Position dist = ecs->GetComponent<Position>(attacker) - ecs->GetComponent<Position>(target);
	return dist.Magnitude() <= 1;
}

void DamageSystem::KillEntity(Entity entity) {
	//[place dropped items and corpse graphic here]
	if (ecs->HasComponent<Active>(entity)) {
		mapSystem->RemoveEntity(entity);
		ecs->RemoveComponent<Active>(entity);
		if (ecs->HasComponent<Renderable>(entity)) { ecs->RemoveComponent<Renderable>(entity); }
		if (ecs->HasComponent<AnimIdle>(entity)) {	ecs->RemoveComponent<AnimIdle>(entity);	}
		if (ecs->HasComponent<AnimMove>(entity)) { ecs->RemoveComponent<AnimMove>(entity); }
		if (ecs->HasComponent<AnimSprite>(entity)) { ecs->RemoveComponent<AnimSprite>(entity); }
	}
}

