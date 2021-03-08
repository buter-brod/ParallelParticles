#pragma once
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

	const std::vector<Effect>& GetEffects() const;

protected:
	Effect* aquireUnusedEffect();
	void start();
	void update();
	void stop();

	bool addToUnusedEffects(unsigned);

private:
	std::vector<Effect> _effects;
	std::set<unsigned> _unusedEffectsSet;

	std::atomic<bool> _stopExplode = false;

	double _timeVault = 0.f;
	double _prevUpdateTime = 0.f;

	std::thread _thread;
	std::atomic<bool> _stopRequested = false;
};

