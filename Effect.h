#pragma once
#include <vector>
#include "Particle.h"

class Effect
{
public:
	Effect();
	std::vector<Particle>& GetParticles() {return _particles;}
	void InitParticles(const Vec2F& pos);

	void Activate();
	void Deactivate();

	bool Update(float dt);

	bool IsAlive() const {return _isAlive;}

protected:

private:
	std::vector<Particle> _particles;
	bool _isAlive = false;
};

