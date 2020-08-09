#pragma once
#include <queue>
#include "System.h"

class AnimationSystem : public System
{
public:
	void AddIdleAnim(Entity entity, Sprite spriteArr[], int l, int frameLengths[]);
	void AddSpriteAnim(FloatPosition pos, Sprite spriteArr[], Tileset tileset, int l, int frameLengths[]);
	void AddSpriteAnim(Entity entity, Sprite spriteArr[], int l, int frameLengths[]);
	void AddMoveAnim(FloatPosition startPos, FloatPosition endPos, float length);
	void AddMoveAnim(Entity entity, FloatPosition startPos, FloatPosition endPos, float length);

	void PlayPendingAnimations();

private:
	void ActivateMoveAnims();
	void ActivateSpriteAnims();

	std::queue<AnimSprite> pendingSpriteAnim = {};
	std::queue<Entity> pendingSpriteAnimEntities = {};
	std::queue<AnimMove> pendingMoveAnim = {};
	std::queue<Entity> pendingMoveAnimEntities = {};
};