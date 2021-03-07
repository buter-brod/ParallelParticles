#pragma once
#include <set>
#include "Effect.h"

class ParticleSystem
{
public:
	ParticleSystem();

	void Start();
	void Update(float dt);
	void Stop();
	void SoftStop();

	std::vector<Effect>& GetEffects();

protected:
	Effect* findUnusedEffect();
	void update();

private:
	std::vector<Effect> _effects;
	std::set<unsigned> _unusedEffects;

	std::atomic<bool> _stopExplode;
	float _timeVault = 0.f;
};

