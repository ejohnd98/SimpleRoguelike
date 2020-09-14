#include "RandomUtil.h"

#include <random>
#include <iostream>
RandomUtil::RandomUtil(int seed) {
	gen = std::mt19937(seed);
}

//adapted from https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
float RandomUtil::GetRandomValue(float a = 0, float b = 1) {
	std::uniform_real_distribution<> dis(a, b);
	return dis(gen);
}

int RandomUtil::GetRandomInt(int a, int b) { //inclusive
	std::uniform_real_distribution<> dis(a, b + 1);
	int rand = (int)dis(gen);

	return rand;
}

//Knuth-Fisher-Yates shuffle algorithm
std::vector<int> RandomUtil::GetRandomSequence(int count) {
	std::vector<int> sequence;
	for (int i = 0; i < count; i++) {
		sequence.push_back(i);
	}
	for (int i = count - 1; i > 0; i--) {
		std::uniform_real_distribution<> dis(0, i + 1);
		int temp = sequence[i];
		int swapIndex = (int)dis(gen);
		sequence[i] = sequence[swapIndex];
		sequence[swapIndex] = temp;
	}
	return sequence;
}

bool RandomUtil::GetRandomBool(float trueChance = 0.5f) { //inclusive
	std::uniform_real_distribution<> dis(0.0, 1.0);
	float rand = dis(gen);
	return rand < trueChance;
}