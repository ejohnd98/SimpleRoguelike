#pragma once
#include "System.h"

class RendererSystem : public System
{
public:
	~RendererSystem();
	void Init();
	void Close();
	void Render();
	bool AnimationPlaying();

private:
	bool animating = false;
	bool LoadMedia();
	void RenderMap(std::shared_ptr<Map> map);
	void RenderTile(FloatPosition pos, Sprite spr, int tileScreenSize);
};