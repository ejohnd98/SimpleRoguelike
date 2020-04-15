#pragma once
#include "System.h"

class RendererSystem : public System
{
public:
	void Init();
	void Close();
	void Render();

private:
	bool LoadMedia();
};