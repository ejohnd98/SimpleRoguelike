#pragma once
#include <queue>
#include "System.h"

class AnimationSystem : public System
{
public:
	void AddIdleAnim(Entity entity, Sprite spriteArr[], int l, int fps);
	void AddSpriteAnim(Entity entity, Sprite spriteArr[], int l, int fps);
	void AddMoveAnim(Entity entity, FloatPosition endPos, float length);

	void PlayPendingAnimations();

private:
	void ActivateMoveAnims();
	void ActivateSpriteAnims();

	std::queue<AnimSprite> pendingSpriteAnim = {};
	std::queue<Entity> pendingSpriteAnimEntities = {};
	std::queue<AnimMove> pendingMoveAnim = {};
	std::queue<Entity> pendingMoveAnimEntities = {};
};