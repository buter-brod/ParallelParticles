#include "ParticleSystem.h"
#include <set>
#include <cassert>

static constexpr unsigned int maxEffectsCount = 2048;

ParticleSystem::ParticleSystem() {
	_effects.resize(maxEffectsCount);
}

std::vector<Effect>& ParticleSystem::GetEffects() {
	return _effects;
}

void ParticleSystem::Start() {

	const Vec2F startPos(0.5f, 0.5f);

	auto* initialEffect = findUnusedEffect();
	initialEffect->Activate();
	initialEffect->InitParticles(startPos);
}

Effect* ParticleSystem::findUnusedEffect() {
	
	for (auto& effect : _effects) {
		if (!effect.IsAlive())
			return &effect;
	}

	return nullptr;
}

void ParticleSystem::Update(float dt) {

	std::set<Particle*> particlesToExplode;

	for (Effect& effect : _effects) {

		if (!effect.IsAlive()) {
			continue;
		}

		effect.Update(dt);

		auto& particles = effect.GetParticles();
		for (auto& particle : particles) {
			const bool alive = particle.GetIsAlive();

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

	for (auto& particle : particlesToExplode) {
		auto* newEffect = findUnusedEffect();

		if (newEffect) {
			newEffect->Activate();
			newEffect->InitParticles(particle->GetPosition());
		} else {
			// sorry, limit reached
		}

		
	}
}

