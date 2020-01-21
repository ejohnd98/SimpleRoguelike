#include "RandomNumber.h"
#include <random>
#include <iostream>

//adapted from https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
float RandomNumber::GetRandomValue(float a = 0, float b = 1){ 
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<> dis(a, b);
	return dis(gen);
}

int RandomNumber::GetRandomInt(int a, int b) { //inclusive
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<> dis(a, b+1);
	int rand = (int)dis(gen);

	//std::cout << rand <<"\n";
	return rand;
}

bool RandomNumber::GetRandomBool(float trueChance = 0.5f) { //inclusive
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<> dis(0.0, 1.0);
	float rand = dis(gen);
	return rand < trueChance;
}