#pragma once
#include <string>
#include "Commands.h"

class Actor
{
public:
	Actor();
	~Actor();

	void Act(class Map* map);
	void GiveCommand(Command command);

	int GetX();
	int GetY();
	class Sprite* GetSprite();
	void SetPos(int x2, int y2);
	std::string GetName();
	void SetName(std::string newName);
private:
	Map* currentMapRef = nullptr;
	int x;
	int y;
	std::string name = "";
	Sprite* actorSprite = nullptr;
};

