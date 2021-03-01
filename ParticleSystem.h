#pragma once
#include <set>

#include "Effect.h"
//#include <set>

class ParticleSystem
{
public:
	ParticleSystem();

	void Start();
	void Update(float dt);

	std::vector<Effect>& GetEffects();

protected:
	Effect* findUnusedEffect();

private:
	std::vector<Effect> _effects;
	//std::set<unsigned> _unusedEffects;
};

