#include "Utils.h"
#include <random>

static const float RANDOM_STRENGTH = 5000.f;

size_t rnd()
{
	static std::default_random_engine rng(std::random_device{}());
	static std::uniform_real_distribution<float> dist(0, RANDOM_STRENGTH);
	return static_cast<size_t>(dist(rng));
}

float rnd01() {
	return float(rnd()) / RANDOM_STRENGTH;
}

float rnd0xf(const float x) {
	return rnd01() * x;
}

unsigned int rnd0xi(const unsigned int x) {
	return rnd() % x;
}

unsigned int rndMinMax(const unsigned int min, const unsigned int max)
{
	return min + rnd0xi(max - min);
}

float rndfMinMax(const float min, const float max) {
	return min + rnd01() * (max - min);
}

bool rndYesNo()
{
	return rnd0xi(2) == 0;
}