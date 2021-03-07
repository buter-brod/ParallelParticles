#pragma once
#include <queue>
#include <set>
#include "Effect.h"

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	void Start();
	void Update(float dt);
	void Stop();
	void SoftStop();

	std::vector<Effect>& GetEffects();

protected:
	Effect* aquireUnusedEffect();
	void update();

	bool addToUnusedEffects(unsigned);

private:
	std::vector<Effect> _effects;
	//std::queue<unsigned> _unusedEffectsQueue;
	std::set<unsigned> _unusedEffectsSet;

	std::atomic<bool> _stopExplode;
	double _timeVault = 0.f;
};

