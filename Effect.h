#pragma once
#include <atomic>
#include <set>
#include <vector>
#include <thread>
#include "Particle.h"

class Effect
{
public:
	Effect();
	Effect(const Effect& other);

	~Effect();
	
	const std::vector<Particle>& GetParticlesToRead() const;
	void Start(const Vec2F& pos);
	
	void Stop();
	void Join();

	void Activate();
	void Deactivate();
	bool IsAlive() const { return _isAlive; }
	
	void Update(float dt);

	std::set<Vec2F> GetExploded();

	unsigned _num = 0; //TODO DEBUG!!! REMOVE!!!

protected:
	std::vector<Particle>& getParticlesToWrite();
	void swapParticleBuffers();
	void swapExplodeBuffers();

	void start(Vec2F pos);

	void checkParticleLife(Particle& p, unsigned index);

private:
	std::vector<Particle> _particles[2];
	std::set<Vec2F> _exploded[2];

	float _timeVault = 0.f;
	float _prevUpdateTime = 0.f;

	std::thread _thread;

	std::atomic<unsigned> _bufferInd = 0;
	std::atomic<unsigned> _explodeInd = 0;
	std::atomic<bool> _isAlive = false;
	std::atomic<bool> _swapExplodesRequested;
	std::atomic<bool> _stopRequested;
};

