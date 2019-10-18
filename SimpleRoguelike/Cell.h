#pragma once

class Cell
{
public:
	Cell();
	~Cell();
	void SetupCell(bool wall);

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
	bool isWall = false;
	class Actor* actor = nullptr;
	class Prop* prop = nullptr;
	class Sprite* cellSprite = nullptr;
};

