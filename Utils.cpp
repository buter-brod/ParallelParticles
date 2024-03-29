#include "Utils.h"

#include <chrono>
#include <random>
//#include <GLFW/glfw3.h>

static const float RANDOM_STRENGTH = 5000.f;

size_t rnd()
{
	static std::default_random_engine rng(std::random_device{}());
	static std::uniform_real_distribution<float> dist(0, RANDOM_STRENGTH);
	return static_cast<size_t>(dist(rng));
}

double getTime() {

	const auto now = std::chrono::system_clock::now();
	const auto duration = now.time_since_epoch();
	const auto mcs = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();

	const double seconds = static_cast<double>(mcs) / 1000.0 / 1000.0;
	return seconds;
	
	//return glfwGetTime();
}

float rnd01() {
	return static_cast<float>(rnd()) / RANDOM_STRENGTH;
}

float rnd0xf(const float x) {
	return rnd01() * x;
}

unsigned int rnd0xi(const unsigned int x) {
	const auto randomNum = rnd();
	const auto result = randomNum % (x + 1);
	return static_cast<unsigned>(result);
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