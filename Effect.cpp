#include "Effect.h"
#include "Utils.h"
#include <cassert>

static constexpr unsigned int maxParticlesPerEffectCount = 64;

static constexpr float particleMinSpeed = 0.13f;
static constexpr float particleMaxSpeed = 0.28f;


Effect::Effect() {
	
	_particles.resize(maxParticlesPerEffectCount);
}

void initParticle(Particle& p, const Vec2F& pos) {
	
	p.Activate();

	p.SetPosition(pos);
	const Vec2F speedVec(rnd01() - 0.5f, rnd01() - 0.5f);
	const float speed = rndfMinMax(particleMinSpeed, particleMaxSpeed);

	p.SetSpeed(speedVec, speed);

	const float r = rnd01();
	const float g = rnd01();
	const float b = rnd01();

	p.SetColor(r, g, b);
}

bool Effect::Update(float dt) {

	bool hasAliveParticles = false;

	for (auto& particle : _particles) {
		const bool alive = particle.IsAlive();

		hasAliveParticles |= alive;

		if (alive) {
			particle.Update(dt);
		}
	}

	return hasAliveParticles;
}

void Effect::InitParticles(const Vec2F& pos) {

	assert(_isAlive);
	
	const unsigned int numParticles = rnd0xi(maxParticlesPerEffectCount);

	for (unsigned pIndex = 0; pIndex < numParticles; ++pIndex) {
		auto& p = _particles[pIndex];
		initParticle(p, pos);
	}
}

void Effect::Activate() {
	assert(!_isAlive);
	_isAlive = true;
}

void Effect::Deactivate() {
	assert(_isAlive);
	_isAlive = false;
}