#include "Particle.h"
#include <cassert>
#include "Utils.h"
#include <cstdint>

#include "Config.h"


Particle::Particle()
{
}

void Particle::SetPosition(const Vec2F& pos)
{
	_info._position = pos;
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
	_info._maxLifetime = rndfMinMax(particleMinLifetime, particleMaxLifetime);
	_info._currLifetime = 0.f;
}

void Particle::SetSpeed(const Vec2F& speedVec, float speed)
{
	_speedVec = speedVec;
	_speed = speed;
}

void Particle::SetColor(float r, float g, float b)
{
	_info._color[0] = r;
	_info._color[1] = g;
	_info._color[2] = b;
}

bool ParticleVisualInfo::GetIsWithinLifetime() const {
	const bool lifetimeValid = _currLifetime < _maxLifetime;
	return lifetimeValid;
}

bool Particle::GetIsWithinLifetime() const {
	
	return _info.GetIsWithinLifetime();
}

void Particle::Update(float dt)
{
	assert(_isAlive);
	_info._position._x += _speedVec._x * dt * _speed;
	_info._position._y += _speedVec._y * dt * _speed;

	_info._currLifetime += dt;
}