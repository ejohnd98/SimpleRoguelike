#pragma once
#include <string>
#include "Commands.h"
#include "Coordinate.h"

enum class ActorState {
	IDLE,
	ATTACK,
	FLEE
};

class Actor
{
public:
	bool playerControlled = false;
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
	int GetFaction();
	int GetSight();
	int GetHealth();
	void SetFaction(int newFaction);
	void SetMapRef(class Map* mapRef);

	//game functions
	void AttackUpon(int attackPower, Actor* attacker);
	void DealDamage(int damage);
	void Kill();
	
protected:
	Map* currentMapRef = nullptr;
	int x = 0;
	int y = 0;
	class Sprite* actorSprite = nullptr;
	bool Move(int x, int y);
	bool ActOnOther(int x, int y);
	Actor* DetermineTarget();
	Command CoordToMoveCommand(int x2, int y2);
	ActorState state = ActorState::ATTACK;
	Coordinate targetLastPos;

	//game parameters
	std::string name = "";
	int health = 10;
	int strength = 3;
	int faction = 0;
	int sight = 12;
};

