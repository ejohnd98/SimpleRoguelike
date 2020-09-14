#pragma once
#include <random>

class RandomUtil
{
public:
	RandomUtil(int seed);

	float GetRandomValue(float a, float b);
	int GetRandomInt(int a, int b);
	std::vector<int> GetRandomSequence(int count);
	bool GetRandomBool(float trueChance);
private:
	std::mt19937 gen;
};

