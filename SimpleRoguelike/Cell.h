#pragma once

class Cell
{
public:
	Cell();
	~Cell();
	void SetupCell(bool wall);

	bool IsOccupied();
	bool ContainsActor();
	class Actor* GetActor();
	class Sprite* GetSprite();
	void SetActor(class Actor* a);
	void RemoveActor();
private:
	bool isWall = false;
	Actor* actor = nullptr;
	Sprite* cellSprite = nullptr;
};

