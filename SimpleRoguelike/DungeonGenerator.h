#pragma once
class DungeonGenerator
{
public:
	DungeonGenerator();
	~DungeonGenerator();

	class DungeonHolder* GenerateDungeon(int depth);
};

