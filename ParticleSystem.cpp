#include "ParticleSystem.h"
#include <set>
#include <random>
#include "Config.h"

ParticleSystem::ParticleSystem() {
	_effects.resize(maxEffectsCount);

	for (unsigned i = 0; i < _effects.size(); i++)
		_effects[i]._num = i;
}

std::vector<Effect>& ParticleSystem::GetEffects() {
	return _effects;
}

void ParticleSystem::Start() {

	//printf("ParticleSystem::Start\n");
	
	const Vec2F startPos(0.5f, 0.5f);

	for(unsigned effInd = 0; effInd < maxEffectsCount; ++effInd)
		_unusedEffects.insert(effInd);

	auto* initialEffect = findUnusedEffect();
	initialEffect->Activate();
	initialEffect->Start(startPos);
}

Effect* ParticleSystem::findUnusedEffect() {

	if (!_unusedEffects.empty()) {
		const auto ind = *_unusedEffects.begin();
		_unusedEffects.erase(_unusedEffects.begin());
		Effect* effect = &_effects[ind];
		return effect;
	}
	
	return nullptr;
}

void ParticleSystem::Update(float dt)
{
	_timeVault += dt;

	while (_timeVault > particleSystemTimeStep) {
		_timeVault -= particleSystemTimeStep;
		update();
	}	
}

void ParticleSystem::Stop() {

	for (auto& effect : _effects)
		effect.Stop();

	for (auto& effect : _effects)
		effect.Join();
}

void ParticleSystem::SoftStop() {
	_stopExplode = true;
}

void ParticleSystem::update() {

	//printf("ParticleSystem::update\n");

	std::vector<Vec2F> explodePosVec;

	for(unsigned effectIndex = 0; effectIndex < _effects.size(); ++effectIndex) {

		Effect& effect = _effects.at(effectIndex);
		
		if (!effect.IsAlive()) {
			_unusedEffects.insert(effectIndex);
			continue;
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
		auto* newEffect = findUnusedEffect();
			
		if (newEffect) {
			//printf("ParticleSystem::update unused effect found %i, will activate now \n", newEffect->_num);
			
			newEffect->Activate();
			newEffect->Start(explodePos);
		} else {
			// sorry, limit reached
		}
	}
}

