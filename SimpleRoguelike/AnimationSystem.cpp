#include <stdio.h>
#include <iostream>
#include <SDL.h>

#include "AnimationSystem.h"
#include "RendererSystem.h"
#include "ECS.h"

extern std::shared_ptr <ECS> ecs;
extern std::shared_ptr<RendererSystem> rendererSystem;

void AnimationSystem::AddIdleAnim(Entity entity, std::vector<Sprite>& sprites, std::vector<int>& frameLengths) {
	AnimIdle anim = {};
	anim.sprites = sprites;
	anim.frameLengths = frameLengths;

	ecs->AddComponent<AnimIdle>(entity, anim);
	ecs->GetComponent<Renderable>(entity).sprite = sprites[0];
}

void AnimationSystem::AddSpriteAnim(FloatPosition pos, std::vector<Sprite>& sprites, Tileset tileset, std::vector<int>& frameLengths) {
	Entity animEntity = ecs->CreateEntity();
	ecs->AddComponent<Renderable>(animEntity, {255, tileset, pos});
	ecs->AddComponent<Active>(animEntity, {});
	ecs->AddComponent<DeleteAfterAnim>(animEntity, {});
	AddSpriteAnim(animEntity, sprites, frameLengths);
}

void AnimationSystem::AddSpriteAnim(Entity entity, std::vector<Sprite>& sprites, std::vector<int>& frameLengths) {
	AnimSprite anim = {};
	anim.loop = false;
	anim.sprites = sprites;
	anim.frameLengths = frameLengths;
	pendingSpriteAnim.push(anim);
	pendingSpriteAnimEntities.push(entity);

	if (!pendingMoveAnim.empty()) {
		ActivateMoveAnims();
	}
}

void AnimationSystem::AddMoveAnim(FloatPosition startPos, FloatPosition endPos, float length) {
	Entity animEntity = ecs->CreateEntity();
	ecs->AddComponent<Renderable>(animEntity, {});
	ecs->AddComponent<Active>(animEntity, {});
	ecs->AddComponent<DeleteAfterAnim>(animEntity, {});
	AddMoveAnim(animEntity, startPos, endPos, length);
}

void AnimationSystem::AddMoveAnim(Entity entity, FloatPosition startPos, FloatPosition endPos, float length) {
	AnimMove anim = {};
	anim.start = startPos;
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

bool AnimationSystem::PendingAnimations() {
	return !pendingMoveAnim.empty() || !pendingSpriteAnim.empty();
}

void AnimationSystem::ActivateMoveAnims() {
	while (!pendingMoveAnim.empty()) {
		rendererSystem->SetAnimationFlag();
		Entity entity = pendingMoveAnimEntities.front();
		if (ecs->HasComponent<AnimMove>(entity)) {
			break;
		}

		pendingMoveAnimEntities.pop();
		AnimMove anim = pendingMoveAnim.front();
		pendingMoveAnim.pop();
		if (ecs->HasComponent<Active>(entity)) {
			ecs->AddComponent<AnimMove>(entity, anim);
		}
	}
}

void AnimationSystem::ActivateSpriteAnims() {
	while (!pendingSpriteAnim.empty()) {
		rendererSystem->SetAnimationFlag();
		Entity entity = pendingSpriteAnimEntities.front();
		if (ecs->HasComponent<AnimSprite>(entity)) {
			break;
		}
		pendingSpriteAnimEntities.pop();
		AnimSprite anim = pendingSpriteAnim.front();
		pendingSpriteAnim.pop();
		if (ecs->HasComponent<Active>(entity)) {
			ecs->AddComponent<AnimSprite>(entity, anim);
		}
	}
}