#include <stdio.h>
#include <iostream>

#include "AnimationSystem.h"
#include "RendererSystem.h"
#include "ECS.h"

extern std::shared_ptr <ECS> ecs;

void AnimationSystem::AddIdleAnim(Entity entity, Sprite spriteArr[], int l, int fps) {
	AnimIdle anim = {};
	anim.length = l;
	anim.framesPerSprite = fps;
	for (int i = 0; i < l; i++) {
		anim.sprites[i] = spriteArr[i];
	}
	ecs->AddComponent<AnimIdle>(entity, anim);
}

void AnimationSystem::AddSpriteAnim(Entity entity, Sprite spriteArr[], int l, int fps) {
	AnimSprite anim = {};
	anim.length = l;
	anim.framesPerSprite = fps;
	anim.loop = false;
	for (int i = 0; i < l; i++) {
		anim.sprites[i] = spriteArr[i];
	}
	pendingSpriteAnim.push(anim);
	pendingSpriteAnimEntities.push(entity);

	if (!pendingMoveAnim.empty()) {
		ActivateMoveAnims();
	}
}

void AnimationSystem::AddMoveAnim(Entity entity, FloatPosition endPos, float length) {
	AnimMove anim = {};
	anim.start = ecs->GetComponent<Position>(entity).ToFloat();
	anim.end = endPos;
	anim.length = length;
	pendingMoveAnim.push(anim);
	pendingMoveAnimEntities.push(entity);

	if (!pendingSpriteAnim.empty()) {
		ActivateSpriteAnims();
	}
}

void AnimationSystem::PlayPendingAnimations() {
	ActivateMoveAnims();
	ActivateSpriteAnims();
}

void AnimationSystem::ActivateMoveAnims() {
	while (!pendingMoveAnim.empty()) {
		Entity entity = pendingMoveAnimEntities.front();
		if (ecs->HasComponent<AnimMove>(entity)) {
			break;
		}

		pendingMoveAnimEntities.pop();
		AnimMove anim = pendingMoveAnim.front();
		pendingMoveAnim.pop();
		ecs->AddComponent<AnimMove>(entity, anim);
	}
}

void AnimationSystem::ActivateSpriteAnims() {
	while (!pendingSpriteAnim.empty()) {
		Entity entity = pendingSpriteAnimEntities.front();
		if (ecs->HasComponent<AnimSprite>(entity)) {
			break;
		}
		pendingSpriteAnimEntities.pop();
		AnimSprite anim = pendingSpriteAnim.front();
		pendingSpriteAnim.pop();
		ecs->AddComponent<AnimSprite>(entity, anim);
	}
}

//keep track of current queued animations and set renderer "animate" bool to true when types of added animations clash
//in other words, animate all pending attack animations before any new move animations (temporatily store that one animation somewhere?)