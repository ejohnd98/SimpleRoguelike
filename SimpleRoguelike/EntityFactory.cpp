#include "EntityFactory.h"
#include "ECS.h"

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr<AnimationSystem> animationSystem;

Entity EntityFactory::CreateActor(ActorType type, bool playerControlled) {
	Entity actor = CreateMapEntity(type.name, type.description, type.defaultSprite);

	ecs->AddComponent(actor, Actor{});
	ecs->AddComponent(actor, type.stats);

	if (playerControlled) {
		ecs->AddComponent(actor, PlayerControlled{});
	} else {
		ecs->AddComponent(actor, AIControlled{});
	}

	animationSystem->AddIdleAnim(actor, type.idleAnim, type.idleAnimlengths);

	return actor;
}

Entity EntityFactory::CreateProp(PropType type) {
	Entity prop = CreateMapEntity(type.name, type.description, type.closedSprite);
	ecs->AddComponent(prop, PropInfo{ type.function });
	switch (type.function) {
	case PropFunction::DOOR: {
		ecs->AddComponent(prop, Openable{ type.closedSprite, type.openedSprite });
		break;
	}
	case PropFunction::STAIRS_DOWN:
	case PropFunction::STAIRS_UP: {
		printf("Creating stairs\n");
		break;
	}
	default:
		break;
	};
	
	return prop;
}


Entity EntityFactory::CreateMapEntity(std::string name, std::string desc, Sprite sprite) {
	Entity newEntity = ecs->CreateEntity();
	ecs->AddComponent(newEntity, Info{name, desc});
	ecs->AddComponent(newEntity, Position{});
	ecs->AddComponent(newEntity, Active{});
	ecs->AddComponent(newEntity, Renderable{sprite});
	return newEntity;
}