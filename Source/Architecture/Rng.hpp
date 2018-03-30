#pragma once
#include <random>

class Rng
{
public:
	Rng() = default;
	~Rng() = default;


	int getRandomInt(int lower_bound, int upper_bound, bool seeded);
	float getRandomFloat(float lower_bound, float upper_bound);
	void setSeed(int new_seed);

private:
	std::random_device rd;
	int seed;
};