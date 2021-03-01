#include "Effect.h"
#include "Utils.h"
#include <cassert>

static constexpr unsigned int maxParticlesPerEffectCount = 64;

static constexpr float particleMinSpeed = 0.03f;
static constexpr float particleMaxSpeed = 0.08f;


Effect::Effect() {
	
	_particles.resize(maxParticlesPerEffectCount);
}

void initParticle(Particle& p, const Vec2F& pos) {
	
	p.Activate();

	p.SetPosition(pos);
	const Vec2F speedVec(rnd01() - 0.5f, rnd01() - 0.5f);
	const float speed = rndfMinMax(particleMinSpeed, particleMaxSpeed);

	p.SetSpeed(speedVec, speed);

	const auto rndColorByte = []() {
		return static_cast<char>(rnd0xi(256));
	};

	p.SetColor(rndColorByte(), rndColorByte(), rndColorByte());
}

void Effect::Update(float dt) {

	bool hasAliveParticles = false;

	for (auto& particle : _particles) {
		const bool alive = particle.GetIsAlive();

		hasAliveParticles |= alive;

		if (alive) {
			particle.Update(dt);
		}
	}

	if (!hasAliveParticles)
		Deactivate();
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