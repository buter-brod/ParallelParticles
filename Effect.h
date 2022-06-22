#pragma once
#include <atomic>
#include <set>
#include <vector>
#include <condition_variable>
#include <thread>
#include "Particle.h"

class Effect
{
public:
	Effect();
	Effect(const Effect& other);

	~Effect();	
	
	void Start(const Vec2F& pos);
	
	void RequestThreadStop();
	void DetachThread();

	bool IsAlive() const { return _isAlive; }
	bool IsThreadRunning() const { return _isThreadRunning; }

	const std::vector<Particle>& GetParticles() const;
	void RequestSwapParticleBuffer() const;
	
	//std::vector<ParticleVisualInfo> GetParticlesInfo() const;
	std::set<Vec2F> GetExploded();

	unsigned _num = 0; //TODO DEBUG!!! REMOVE!!!

protected:
	const std::vector<Particle>& getParticlesToRead() const;
	std::vector<Particle>& getParticlesToWrite();
	void swapParticleBuffers();
	void swapExplodeBuffers();

	void start(Vec2F pos);
	void update(double dt);

	void deactivate();

	void checkParticleLife(Particle& p, unsigned index);

private:
	std::vector<Particle> _particles[2];
	std::set<Vec2F> _exploded[2];

	double _timeVault = 0.f;
	double _prevUpdateTime = 0.f;

	std::thread _thread;

	std::atomic<unsigned> _particleBufferInd = 0;
	std::atomic<unsigned> _explodeInd = 0;
	std::atomic<bool> _isAlive = false;
	std::atomic<bool> _isThreadRunning = false;
	std::atomic<bool> _swapExplodesRequested = false;
	mutable std::atomic<bool> _swapBuffersRequested = false;
	
	std::atomic<bool> _stopRequested = false;
};

