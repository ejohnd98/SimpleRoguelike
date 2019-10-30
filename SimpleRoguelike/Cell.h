#pragma once

class Cell
{
public:
	Cell(int xi, int yi);
	~Cell();
	void SetupCell(bool wall);

	int GetX();
	int GetY();
	bool IsOccupied();
	bool ContainsActor();
	bool ContainsProp();
	class Actor* GetActor();
	class Prop* GetProp();
	class Sprite* GetSprite();
	void SetActor(class Actor* a);
	void SetProp(class Prop* p);
	void RemoveActor();
	void RemoveProp();
private:
	int x = -1;
	int y = -1;
	bool isWall = false;
	class Actor* actor = nullptr;
	class Prop* prop = nullptr;
	class Sprite* cellSprite = nullptr;
};

