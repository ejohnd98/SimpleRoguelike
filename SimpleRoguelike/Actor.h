#pragma once
#include <string>
#include "Commands.h"

class Actor
{
public:
	Actor();
	~Actor();

	void Act();
	bool GiveCommand(Command command);

	int GetX();
	int GetY();
	class Sprite* GetSprite();
	void SetSprite(class Sprite* spr);
	void SetPos(int x2, int y2);
	std::string GetName();
	void SetName(std::string newName);
	void SetMapRef(class Map* mapRef);
private:
	Map* currentMapRef = nullptr;
	int x = 0;
	int y = 0;
	std::string name = "";
	class Sprite* actorSprite = nullptr;
	bool Move(int x, int y);
};

