#pragma once
#include <atomic>
#include <set>
#include <vector>
#include "Particle.h"

class Effect
{
public:
	Effect();
	Effect(const Effect& other);
	
	std::vector<Particle>& GetParticles();
	void InitParticles(const Vec2F& pos);

	void Activate();
	void Deactivate();

	void Update(float dt);

	bool IsAlive() const {return _isAlive;}

	std::set<Vec2F> GetExploded();

protected:
	std::vector<Particle>& getActiveParticles();
	void swapBuffers();
	void swapExplodes();

	void checkParticleLife(Particle& p, unsigned index);

private:
	std::vector<Particle> _particles[2];
	std::set<Vec2F> _exploded[2];
	
	std::atomic<unsigned> _bufferInd = 0;
	std::atomic<unsigned> _explodeInd = 0;
	
	std::atomic<bool> _isAlive = false;
};

