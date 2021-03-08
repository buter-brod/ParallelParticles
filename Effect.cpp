#include "Effect.h"
#include "Utils.h"
#include <cassert>

#include "Config.h"

Effect::Effect(const Effect& other) {

	// had to "implement" manual copy constructor just because of non-copyable _bufferInd. Should never be called
	assert(false);
}

Effect::~Effect() {

	if (_isAlive)
		deactivate();
	
	Join();
}

const std::vector<Particle>& Effect::getParticlesToRead() const {

	return _particles[1 - _particleBufferInd];
}

std::vector<Particle>& Effect::getParticlesToWrite()
{
	return _particles[_particleBufferInd];
}

Effect::Effect() {
	
	_particles[0].resize(maxParticlesPerEffectCount);
	_particles[1].resize(maxParticlesPerEffectCount);
}

void initParticle(Particle& p, const Vec2F& pos) {

	//printf("effect initParticle at %f %f \n", pos._x, pos._y);

	p.SetPosition(pos);

	const Vec2F speedVec(rnd01() - 0.5f, rnd01() - 0.5f);
	const float speed = rndfMinMax(particleMinSpeed, particleMaxSpeed);
	p.SetSpeed(speedVec, speed);

	const float r = rnd01();
	const float g = rnd01();
	const float b = rnd01();
	p.SetColor(r, g, b);

	p.Activate();
}

std::set<Vec2F> Effect::GetExploded() {

	std::set<Vec2F> readExplodeSet;
	
	if (_swapExplodesRequested)
		return readExplodeSet;
	
	readExplodeSet = _exploded[1 - _explodeInd];
	_swapExplodesRequested = true;
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
		//printf("effect %i, particle %i deactivated due to position \n", _num, index);
		p.Deactivate();
		return;
	}

	const bool dieOrExplodeTime = !p.GetIsWithinLifetime();
	if (dieOrExplodeTime)
	{
		if (p.GetCanExplode()) {
			//printf("effect %i, particle %i added to explosion list and deactivated \n", _num, index);
			_exploded[_explodeInd].insert(pos);
		}
		else 
		{
			//printf("effect %i, particle %i deactivated\n", _num, index);
		}

		p.Deactivate();
	}
}

void Effect::update(const double dt) {

	//printf("effect %i Update\n", _num);

	bool hasAliveParticlesNow = false;

	auto& particlesToWrite = getParticlesToWrite();

	for (unsigned index = 0; index < particlesToWrite.size(); ++index) {

		auto& particleToWrite = particlesToWrite.at(index);
		const bool alive = particleToWrite.IsAlive();
		if (alive) {

			particleToWrite.Update(dt);
			checkParticleLife(particleToWrite, index);

			hasAliveParticlesNow |= particleToWrite.IsAlive();
		}
	}

	if (!hasAliveParticlesNow) {
		const bool noExplosionsPending = _exploded[0].empty() && _exploded[1].empty();
		if (noExplosionsPending) {
			deactivate();
		}
	}
}

void Effect::Join() {

	if (_thread.joinable())
		_thread.join();
}

void Effect::Stop() {
	_stopRequested = true;
}

const std::vector<Particle>& Effect::GetParticles() const {
	const auto& particles = getParticlesToRead();
	return particles;
}

void Effect::RequestSwapParticleBuffer() const {
	
	_swapBuffersRequested = true;
}

void Effect::swapExplodeBuffers() {

	_explodeInd ^= 1;
	_exploded[_explodeInd].clear();
	_swapExplodesRequested = false;

	//printf("effect %i swapExplodes, now %i \n", _num, _explodeInd.load());
}

void Effect::swapParticleBuffers() {

	_particleBufferInd ^= 1;
	_swapBuffersRequested = false;

	auto& toWrite = getParticlesToWrite();
	const auto& toRead = getParticlesToRead();
	toWrite = toRead;

	//printf("effect %i swapBuffers, now %i \n", _num, _bufferInd.load());
}

void Effect::start(const Vec2F pos) {

	//printf("effect %i start\n", _num);

	const unsigned int numParticlesToGenerate = rndMinMax(1, maxParticlesPerEffectCount);
	auto& particles = getParticlesToWrite();

	for (unsigned pIndex = 0; pIndex < numParticlesToGenerate; ++pIndex)
	{
		auto& p = particles[pIndex];
		initParticle(p, pos);
	}

	swapParticleBuffers();

	_prevUpdateTime = getTime();
	_timeVault = 0;

	while(_isAlive && !_stopRequested) {

		const auto currTime = getTime();
		const auto dt = currTime - _prevUpdateTime;

		_timeVault += dt * effectSimTimeScale;
		_prevUpdateTime = currTime;

		if (_timeVault < effectSimTimeStep) {
			const double sleepTime = effectSimTimeStep - _timeVault;
			const auto sleepMs = static_cast<unsigned>(sleepTime * 1000.0);
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
			continue;
		}

		while (_isAlive && _timeVault >= effectSimTimeStep && !_stopRequested)
		{
			_timeVault -= effectSimTimeStep;
			update(effectSimTimeStep);
		}

		if (_isAlive)
		{
			if (_swapBuffersRequested)
				swapParticleBuffers();
			
			if (_swapExplodesRequested)
				swapExplodeBuffers();
		}
	}
}

void Effect::Start(const Vec2F& pos) {

	//printf("effect %i Start\n", _num);

	Join();
	
	assert(!_isAlive);
	_isAlive = true;
	_stopRequested = false;
	
	_thread = std::thread([this, pos](){start(pos);});
}

void Effect::deactivate() {
	assert(_isAlive);
	_isAlive = false;

	swapParticleBuffers();
	
	Stop();

	//printf("effect %i deactivated\n", _num);
}

