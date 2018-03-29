#pragma once
#include <random>

class Rng
{
public:
	Rng() = default;
	~Rng() = default;

	int getRandomInt(int lower_bound, int upper_bound);
	float getRandomFloat(float lower_bound, float upper_bound);

private:
	std::random_device rd;

};