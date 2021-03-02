#pragma once
#include <queue>
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
	void update();

private:
	std::vector<Effect> _effects;
	std::set<unsigned> _unusedEffects;
	float _timeVault = 0.f;
};

