#pragma once
class FieldOfView
{
public:
	static void SetFOV(class Map* m, class Actor* p);
	static bool CanSeeCell(Map* m, Actor* s, int x, int y);
	static bool InSight(int x1, int y1, int x2, int y2, int sight);
};

