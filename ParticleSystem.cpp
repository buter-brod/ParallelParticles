#include "ParticleSystem.h"
#include <set>
#include <cassert>
#include <random>

static constexpr unsigned int maxEffectsCount = 2048;

static constexpr float timeStep = 0.01f;

ParticleSystem::ParticleSystem() {
	_effects.resize(maxEffectsCount);
}

std::vector<Effect>& ParticleSystem::GetEffects() {
	return _effects;
}

void ParticleSystem::Start() {

	const Vec2F startPos(0.5f, 0.5f);

	for(unsigned effInd = 0; effInd < maxEffectsCount; ++effInd)
		_unusedEffects.insert(effInd);

	auto* initialEffect = findUnusedEffect();
	initialEffect->Activate();
	initialEffect->InitParticles(startPos);
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

	while (_timeVault > timeStep) {
		_timeVault -= timeStep;
		update();
	}	
}

void ParticleSystem::update() {

	const float dt = timeStep;
	std::vector<Vec2F> explodePosVec;

	for(unsigned effectIndex = 0; effectIndex < _effects.size(); ++effectIndex) {

		Effect& effect = _effects.at(effectIndex);
		
		if (!effect.IsAlive()) {
			_unusedEffects.insert(effectIndex);
			continue;
		}

		effect.Update(dt);

		const auto& exploded = effect.GetExploded();

		for (const auto& exlodedPos : exploded) {
			explodePosVec.push_back(exlodedPos);
		}		
	}

	std::shuffle(explodePosVec.begin(), explodePosVec.end(), std::default_random_engine());

	for (const auto& explodePos : explodePosVec) {
		auto* newEffect = findUnusedEffect();

		if (newEffect) {
			newEffect->Activate();
			newEffect->InitParticles(explodePos);
		} else {
			// sorry, limit reached
		}
	}
}

