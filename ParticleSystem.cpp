#include "ParticleSystem.h"
#include <set>
#include <random>
#include "Config.h"
#include "Utils.h"

ParticleSystem::ParticleSystem() {
	_effects.resize(maxEffectsCount);

	for (unsigned i = 0; i < _effects.size(); i++)
		_effects[i]._num = i;
}

ParticleSystem::~ParticleSystem() {
	Stop();
}

const std::vector<Effect>& ParticleSystem::GetEffects() const {
	return _effects;
}

void ParticleSystem::start() {

	const Vec2F startPos(0.5f, 0.5f);

	for(unsigned effInd = 0; effInd < maxEffectsCount; ++effInd)
		addToUnusedEffects(effInd);

	auto* initialEffect = aquireUnusedEffect();
	initialEffect->Start(startPos);

	_prevUpdateTime = getTime();
	_timeVault = 0;

	while(!_stopRequested) {

		const auto currTime = getTime();
		const auto dt = currTime - _prevUpdateTime;

		_timeVault += dt * particleSystemTimeScale;
		_prevUpdateTime = currTime;

		if (_timeVault < particleSystemTimeStep) {
			const double sleepTime = particleSystemTimeStep - _timeVault;
			const auto sleepMs = static_cast<unsigned>(sleepTime * 1000.0);
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
			continue;
		}
		
		while (_timeVault >= particleSystemTimeStep && !_stopRequested)
		{
			_timeVault -= particleSystemTimeStep;
			update();
		}		
	}

	stop();
}

void ParticleSystem::Start() {

	_thread = std::thread([this](){start();});
}

Effect* ParticleSystem::aquireUnusedEffect() {

	if (!_unusedEffectsSet.empty()) {
		
		const auto ind = *_unusedEffectsSet.begin();
		_unusedEffectsSet.erase(_unusedEffectsSet.begin());
		Effect* effect = &_effects[ind];
		return effect;
	}
	
	return nullptr;
}

void ParticleSystem::Stop() {

	_stopRequested = true;
	if (_thread.joinable())
		_thread.join();
}

void ParticleSystem::stop() {

	for (auto& effect : _effects)
		effect.RequestThreadStop();

	for (auto& effect : _effects)
		effect.DetachThread();
}

void ParticleSystem::SoftStop() {
	_stopExplode = true;
}

bool ParticleSystem::addToUnusedEffects(const unsigned index) {

	_unusedEffectsSet.insert(index);
	return true;
}

void ParticleSystem::update() {

	//printf("ParticleSystem::update\n");

	std::vector<Vec2F> explodePosVec;

	for(unsigned effectIndex = 0; effectIndex < _effects.size(); ++effectIndex) {

		Effect& effect = _effects.at(effectIndex);

		if (!effect.IsAlive())
		{
			if (!effect.IsThreadRunning())
			{
				addToUnusedEffects(effectIndex);
				continue;
			}
		}

		const auto& exploded = effect.GetExploded();
		for (const auto& exlodedPos : exploded)
			explodePosVec.push_back(exlodedPos);
	}

	if (_stopExplode) {
		explodePosVec.clear();
	}

	std::shuffle(explodePosVec.begin(), explodePosVec.end(), std::default_random_engine());	

	for (const auto& explodePos : explodePosVec) {
		auto* newEffect = aquireUnusedEffect();
			
		if (newEffect) {
			//printf("ParticleSystem::update unused effect found %i, will activate now \n", newEffect->_num);
			
			newEffect->Start(explodePos);
		} else {
			// sorry, limit reached
		}
	}

	if (_unusedEffectsSet.size() == maxEffectsCount) {
		_stopRequested = true;
	}
}

