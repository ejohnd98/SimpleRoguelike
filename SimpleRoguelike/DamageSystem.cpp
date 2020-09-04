#include <stdio.h>
#include <iostream>

#include "DamageSystem.h"
#include "ECS.h"

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr <AnimationSystem> animationSystem;
extern std::shared_ptr <MapSystem> mapSystem;
extern std::shared_ptr<LogSystem> logSystem;

void DamageSystem::Attack(Entity attacker, Entity target) {
	Stats& attackerStats = ecs->GetComponent<Stats>(attacker);
	Stats& targetStats = ecs->GetComponent<Stats>(target);

	int damage = std::max( attackerStats.atk - targetStats.def , 0);
	EventInfo attackEvent;

	attackEvent.source = attacker;
	attackEvent.target = target;
	attackEvent.damageDealt = damage;
	attackEvent.type = EventType::ATTACK;
	attackEvent.killed = DealDamage(target, damage);
	logSystem->AddLog(attackEvent);
}

bool DamageSystem::DealDamage(Entity target, int damage) {
	Stats& targetStats = ecs->GetComponent<Stats>(target);
	targetStats.hp -= damage;

	if (targetStats.hp <= 0) {
		KillEntity(target);
		return true;
	}
	return false;
}

bool DamageSystem::WithinAttackRange(Entity attacker, Entity target) {
	Position dist = ecs->GetComponent<Position>(attacker) - ecs->GetComponent<Position>(target);
	return dist.Magnitude() <= 1;
}

void DamageSystem::KillEntity(Entity entity) {
	//[place dropped items and corpse graphic here]
	FloatPosition pos = ecs->GetComponent<Position>(entity).ToFloat();
	std::vector<Sprite> sprites{ 0, 1, 2, 3, 4, 5 };
	std::vector<int> lengths{ 2, 2, 3, 3, 4, 4 };

	animationSystem->AddSpriteAnim(pos, sprites, (Tileset)"32x32_boom", lengths);

	if (ecs->HasComponent<Active>(entity)) {
		mapSystem->RemoveEntity(entity);
		ecs->RemoveComponent<Active>(entity);
		if (ecs->HasComponent<Renderable>(entity)) { ecs->RemoveComponent<Renderable>(entity); }
		if (ecs->HasComponent<AnimIdle>(entity)) {	ecs->RemoveComponent<AnimIdle>(entity);	}
		if (ecs->HasComponent<AnimMove>(entity)) { ecs->RemoveComponent<AnimMove>(entity); }
		if (ecs->HasComponent<AnimSprite>(entity)) { ecs->RemoveComponent<AnimSprite>(entity); }
	}
}

