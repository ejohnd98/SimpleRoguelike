#pragma once
#include "System.h"

class RendererSystem : public System
{
public:
	~RendererSystem();
	void Init();
	void Close();
	void Render();

private:
	bool LoadMedia();
	void RenderMap(std::shared_ptr<Map> map);
	void RenderTile(FloatPosition pos, Sprite spr, int tileScreenSize);
};