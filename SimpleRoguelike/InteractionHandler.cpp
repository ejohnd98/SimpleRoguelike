#include <stdio.h>
#include <iostream>

#include "InteractionHandler.h"
#include "ECS.h"

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr<MapSystem> mapSystem;
extern std::shared_ptr<AnimationSystem> animationSystem; //shouldn't need later (temp)
extern std::shared_ptr<TurnSystem> turnSystem;
extern std::shared_ptr<DamageSystem> damageSystem;

bool InteractionHandler::PerformAction(Entity entity, Position target, InteractType interactType) {
	bool actionPerformed = false;
	Entity other = mapSystem->GetEntityAt(target);

	switch (interactType) {
		case InteractType::WAIT:
			actionPerformed = true;
			break;
		case InteractType::MOVE:
			if (entity != NULL_ENTITY && !mapSystem->BlocksMovement(target, false)) {
				animationSystem->AddMoveAnim(entity, ecs->GetComponent<Position>(entity).ToFloat(), target.ToFloat(), 0.20f);
				mapSystem->MoveEntity(entity, target);
				actionPerformed = true;
			}
			break;
		case InteractType::ATTACK:
			if (entity != NULL_ENTITY && other != NULL_ENTITY && ecs->HasComponent<Stats>(other)) {
				std::vector<Sprite> sprites{ 112,113,114,115,116 };
				std::vector<int> lengths{ 3, 1, 1, 2, 2 };
				animationSystem->AddSpriteAnim(target.ToFloat(), sprites, MAIN_TILESET, lengths);

				damageSystem->Attack(entity, other);
				actionPerformed = true;
			}
			break;
		case InteractType::OPEN:
			if (other!=NULL_ENTITY && ecs->HasComponent<Openable>(other)) {
				auto& openable = ecs->GetComponent<Openable>(other);
				auto& renderable = ecs->GetComponent<Renderable>(other);
				openable.isOpen = true;
				renderable.sprite = openable.openedSprite;
				actionPerformed = true;
			}
			break;
		case InteractType::CLOSE:
			if (other != NULL_ENTITY && ecs->HasComponent<Openable>(other)) {
				auto& openable = ecs->GetComponent<Openable>(other);
				auto& renderable = ecs->GetComponent<Renderable>(other);
				openable.isOpen = false;
				renderable.sprite = openable.closedSprite;
				actionPerformed = true;
			}
			break;
		default:
			break;	
	}

	if (actionPerformed && entity != NULL_ENTITY) {
		turnSystem->AddDebt(entity, interactType);
	}

	return actionPerformed;
}