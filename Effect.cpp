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

std::vector<ParticleVisualInfo> Effect::GetParticlesInfo() const {

	// todo: guard! prevert swap buffers while copying

	_copyingFromBuffer = 1 - _particleBufferInd;

	const auto& particlesToRead = _particles[_copyingFromBuffer];

	std::vector<ParticleVisualInfo> particlesInfoVec;
	particlesInfoVec.reserve(particlesToRead.size());

	for (const auto& particle : particlesToRead) {
		if (particle.IsAlive())
			particlesInfoVec.push_back(particle.GetVisualInfo());
	}

	_copyingFromBuffer = -1;
	_copyingDoneCondition.notify_one();
	
	return particlesInfoVec;
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

	//const auto explodedSize = readExplodeSet.size();
	//if (explodedSize > 0)
		//printf("effect %i, GetExploded called, returning %i explodes \n", _num, explodedSize);

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

	if (_swapExplodesRequested)
	{
		swapExplodeBuffers();
		_exploded[_explodeInd].clear();
		_swapExplodesRequested = false;
	}
	
	bool hadAliveParticlesBefore = false;
	bool hasAliveParticlesNow = false;

	if (_copyingFromBuffer == _particleBufferInd) {

		//const auto t1 = getTime();
		std::unique_lock<std::mutex> copyGuard(_copyingMutex);
		_copyingDoneCondition.wait(copyGuard, [this]{return _copyingFromBuffer != _particleBufferInd;});
		//const auto t2 = getTime();
		//const auto ddt = t2 - t1;
		//printf("WOW waited for %.8f seconds because of buffer desync! \n", ddt);
	}

	const auto& particlesToRead = getParticlesToRead();
	auto& particlesToWrite = getParticlesToWrite();

	for (unsigned index = 0; index < particlesToRead.size(); ++index) {

		auto& particleToWrite = particlesToWrite.at(index);

		// copying
		particleToWrite = particlesToRead.at(index);
		
		const bool alive = particleToWrite.IsAlive();
		if (alive) {

			hadAliveParticlesBefore |= true;

			particleToWrite.Update(dt);
			checkParticleLife(particleToWrite, index);

			hasAliveParticlesNow |= particleToWrite.IsAlive();
		}
	}

	if (hadAliveParticlesBefore || hasAliveParticlesNow) {
		swapParticleBuffers();
	}

	if (!hasAliveParticlesNow) {
		const bool noExplosionsPending = _exploded[_explodeInd].empty() && _exploded[1 - _explodeInd].empty();
		if (noExplosionsPending) {
			swapParticleBuffers();
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

void Effect::swapExplodeBuffers() {

	_explodeInd ^= 1;

	//printf("effect %i swapExplodes, now %i \n", _num, _explodeInd.load());
}

void Effect::swapParticleBuffers() {

	//std::unique_lock<std::mutex> lockBuffer(_copyingMutex);
	//_copyingDoneCondition.wait(lockBuffer, [this]{return _copyingFromBuffer == -1;});
	
	_particleBufferInd ^= 1;

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
		const auto dt = static_cast<float>(currTime - _prevUpdateTime);

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
	}
}

void Effect::Start(const Vec2F& pos) {

	//printf("effect %i Start\n", _num);

	//const auto t1 = getTime();

	Join();
	
	//const auto t2 = getTime();
	//const auto ddt = t2 - t1;
	//printf("WOW waited for %.8f seconds for thread to join! \n", ddt);	
	
	assert(!_isAlive);
	_isAlive = true;
	_stopRequested = false;
	
	_thread = std::thread([this, pos](){start(pos);});
}

void Effect::deactivate() {
	assert(_isAlive);
	_isAlive = false;
	
	Stop();

	//printf("effect %i deactivated\n", _num);
}

