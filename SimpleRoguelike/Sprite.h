#pragma once
class Sprite
{
public:
	Sprite(int i);
	~Sprite();
	int GetIndex();
	void SetIndex(int i);
private:
	int index = 0;
};
