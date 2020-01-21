#pragma once
class MapGenerator
{
public:
	class Map* GenerateNewMap(int w, int h);
	void GenerateMap(class Map* map, int w, int h);
	void PopulateDungeon(int floor, int numOfFloors, Map* map);
};

