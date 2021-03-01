#include "ParticleSystem.h"
#include <set>
#include <cassert>

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
		_unusedEffects.push(effInd);

	auto* initialEffect = findUnusedEffect();
	initialEffect->Activate();
	initialEffect->InitParticles(startPos);
}

Effect* ParticleSystem::findUnusedEffect() {

	if (!_unusedEffects.empty()) {
		const auto ind = _unusedEffects.front();
		_unusedEffects.pop();
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
	std::set<Particle*> particlesToExplode;

	for(unsigned effectIndex = 0; effectIndex < _effects.size(); ++effectIndex) {

		Effect& effect = _effects.at(effectIndex);
		
		if (!effect.IsAlive()) {
			continue;
		}

		const bool effectAlive = effect.Update(dt);

		if (!effectAlive) {
			effect.Deactivate();
			_unusedEffects.push(effectIndex);
			continue;
		}

		auto& particles = effect.GetParticles();
		for (auto& particle : particles) {
			const bool alive = particle.IsAlive();
			if (!alive)
				continue;

			const auto& pos = particle.GetPosition();
			const bool outOfBounds =
				pos._x > 1.f ||
				pos._y > 1.f ||
				pos._x < 0.f ||
				pos._y < 0.f;

			if (outOfBounds) {
				particle.Deactivate();
				continue;
			}

			const bool dieOrExplodeTime = !particle.GetIsWithinLifetime();
			if (dieOrExplodeTime) {
				if (particle.GetCanExplode())
					particlesToExplode.insert(&particle);

				particle.Deactivate();
			}
		}
	}

	for (const auto& particle : particlesToExplode) {
		auto* newEffect = findUnusedEffect();

		if (newEffect) {
			newEffect->Activate();
			newEffect->InitParticles(particle->GetPosition());
		} else {
			// sorry, limit reached
		}
	}
}

