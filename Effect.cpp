#include "Effect.h"
#include "Utils.h"
#include <cassert>

static constexpr unsigned int maxParticlesPerEffectCount = 64;

static constexpr float particleMinSpeed = 0.1f;
static constexpr float particleMaxSpeed = 0.3f;

Effect::Effect(const Effect& other) {

	// had to "implement" manual copy constructor just because of non-copyable _bufferInd. Should never be called
	assert(false);
}

std::vector<Particle>& Effect::GetParticles() {

	const auto readBufferInd = 1 - _bufferInd;
	return _particles[readBufferInd];
}

std::vector<Particle>& Effect::getActiveParticles()
{
	return _particles[_bufferInd];
}

Effect::Effect() {
	
	_particles[0].resize(maxParticlesPerEffectCount);
	_particles[1].resize(maxParticlesPerEffectCount);
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

std::set<Vec2F> Effect::GetExploded() {

	const auto readExplodeInd = 1 - _explodeInd;
	const auto& readExplodeSet = _exploded[readExplodeInd];
	return readExplodeSet;
}

void Effect::checkParticleLife(Particle& p, const unsigned index) {

	const auto& pos = p.GetPosition();
	const bool outOfBounds =
		pos._x > 1.f ||
		pos._y > 1.f ||
		pos._x < 0.f ||
		pos._y < 0.f;

	if (outOfBounds) {
		p.Deactivate();
		return;
	}	

	const bool dieOrExplodeTime = !p.GetIsWithinLifetime();
	if (dieOrExplodeTime)
	{
		if (p.GetCanExplode())
			_exploded[_explodeInd].insert(pos);

		p.Deactivate();
	}
}

void Effect::Update(const float dt) {

	bool hasAliveParticles = false;

	const auto& particlesToRead = GetParticles();
	auto& particlesToWrite = getActiveParticles();

	_exploded[_explodeInd].clear();

	for (unsigned index = 0; index < particlesToRead.size(); ++index) {

		const auto& particleToRead = particlesToRead.at(index);

		auto& particleToWrite = particlesToWrite.at(index);
		particleToWrite = particleToRead;
		
		const bool alive = particleToRead.IsAlive();
		if (alive) {
			particleToWrite.Update(dt);
			checkParticleLife(particleToWrite, index);
			hasAliveParticles |= particleToWrite.IsAlive();
		}
	}

	if (!hasAliveParticles && _exploded[_explodeInd].empty())
		Deactivate();
	
	swapBuffers();
	swapExplodes();
}

void Effect::swapExplodes() {
	_explodeInd ^= 1;
}

void Effect::swapBuffers() {
	_bufferInd ^= 1;
}

void Effect::InitParticles(const Vec2F& pos) {

	assert(_isAlive);
	
	const unsigned int numParticles = rndMinMax(1, maxParticlesPerEffectCount);

	auto& particles = getActiveParticles();
	
	for (unsigned pIndex = 0; pIndex < numParticles; ++pIndex) {
		auto& p = particles[pIndex];
		initParticle(p, pos);
	}
	
	swapBuffers();
}

void Effect::Activate() {
	assert(!_isAlive);
	_isAlive = true;
}

void Effect::Deactivate() {
	assert(_isAlive);
	_isAlive = false;

	const unsigned bufferInd = _bufferInd;

	// if deactivated, make both buffers equal
	_particles[1 - bufferInd] = _particles[bufferInd];
}