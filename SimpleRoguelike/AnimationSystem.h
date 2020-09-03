#pragma once
#include <queue>
#include "System.h"

class AnimationSystem : public System
{
public:
	void AddIdleAnim(Entity entity, std::vector<Sprite>& sprites, std::vector<int>& frameLengths);
	void AddSpriteAnim(FloatPosition pos, std::vector<Sprite>& sprites, Tileset tileset, std::vector<int>& frameLengths);
	void AddSpriteAnim(Entity entity, std::vector<Sprite>& sprites, std::vector<int>& frameLengths);
	void AddMoveAnim(FloatPosition startPos, FloatPosition endPos, float length);
	void AddMoveAnim(Entity entity, FloatPosition startPos, FloatPosition endPos, float length);

	void PlayPendingAnimations();
	bool PendingAnimations();

private:
	void ActivateMoveAnims();
	void ActivateSpriteAnims();

	std::queue<AnimSprite> pendingSpriteAnim = {};
	std::queue<Entity> pendingSpriteAnimEntities = {};
	std::queue<AnimMove> pendingMoveAnim = {};
	std::queue<Entity> pendingMoveAnimEntities = {};
};