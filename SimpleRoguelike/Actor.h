#pragma once
#include <string>
#include "Commands.h"

class Actor
{
public:
	Actor();
	Actor(std::string n, int sprIndex);
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

	//game functions
	void AttackUpon(int attackPower, Actor* attacker);
	void DealDamage(int damange);
protected:
	Map* currentMapRef = nullptr;
	int x = 0;
	int y = 0;
	class Sprite* actorSprite = nullptr;
	bool Move(int x, int y);
	bool ActOnOther(int x, int y);

	//game parameters
	std::string name = "";
	int health = 10;
	int strength = 3;
};

