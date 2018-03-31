#include "Rng.hpp"

//STD
#include <chrono>
#include <iostream>

Rng::Rng()
{
	last_seed = (int)std::chrono::system_clock::now().time_since_epoch().count();
	setSeed(last_seed);
}

int Rng::getRandomInt(int lower_bound, int upper_bound)
{
	std::uniform_int_distribution<int> random(lower_bound, upper_bound);
	return random(mt);
}

float Rng::getRandomFloat(float lower_bound, float upper_bound)
{
	std::uniform_real_distribution<float> random(lower_bound, upper_bound);
	return random(mt);
}

void Rng::setSeed(int seed)
{
	last_seed = seed;
	mt.seed(last_seed);
	std::cout << "RNG Seed: " << last_seed << "\n";
}
