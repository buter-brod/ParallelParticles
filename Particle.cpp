#include "Particle.h"
#include <cassert>
#include "Utils.h"
#include <cstdint>

static constexpr float particleMinLifetime = 2.f;
static constexpr float particleMaxLifetime = 10.f;

static constexpr float particleExplodeProbability = 0.75f;

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
}

void Particle::SetSpeed(const Vec2F& speedVec, float speed)
{
	assert(_isAlive);
	_speedVec = speedVec;
	_speed = speed;
}

void Particle::SetColor(char r, char g, char b)
{
	_color[0] = r;
	_color[1] = g;
	_color[2] = b;
}

void Particle::GetColor(float& r, float& g, float& b) {
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