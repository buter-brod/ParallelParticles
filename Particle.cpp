#include "Particle.h"
#include <cassert>
#include "Utils.h"
#include <cstdint>

static constexpr float particleMinLifetime = 1.f;
static constexpr float particleMaxLifetime = 3.f;
static constexpr float particleExplodeProbability = 0.125f;

Particle::Particle()
{
}

void Particle::SetPosition(const Vec2F& pos)
{
	_position = pos;
}

void Particle::Deactivate()
{
	assert(_isAlive);
	_isAlive = false;
}

void Particle::Activate()
{
	assert(!_isAlive);
	_isAlive = true;

	_canExplode = rnd01() < particleExplodeProbability;
	_maxLifetime = rndfMinMax(particleMinLifetime, particleMaxLifetime);
	_currLifetime = 0.f;
}

void Particle::SetSpeed(const Vec2F& speedVec, float speed)
{
	_speedVec = speedVec;
	_speed = speed;
}

void Particle::SetColor(float r, float g, float b)
{
	_color[0] = r;
	_color[1] = g;
	_color[2] = b;
}

void Particle::GetColor(float& r, float& g, float& b) const {
	r = _color[0];
	g = _color[1];
	b = _color[2];
}

bool Particle::GetIsWithinLifetime() const {
	
	const bool lifetimeValid = _currLifetime < _maxLifetime;
	return lifetimeValid;
}

void Particle::Update(float dt)
{
	assert(_isAlive);
	_position._x += _speedVec._x * dt * _speed;
	_position._y += _speedVec._y * dt * _speed;

	_currLifetime += dt;
}