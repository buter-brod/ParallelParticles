#pragma once
#include <cstdint>

struct Vec2F
{
	Vec2F() = default;
	Vec2F(float x, float y) : _x(x), _y(y) {}
	float _x = 0.f;
	float _y = 0.f;

	bool operator<(const Vec2F& other) const {

		if (_x == other._x) {
			return _y < other._y;
		}
		
		return _x < other._x;
	}
};

class Particle
{
public:
	Particle();

	void SetSpeed(const Vec2F& speedVec, float speed);

	void SetPosition(const Vec2F& pos);
	const Vec2F& GetPosition() const { return _position; }

	void SetColor(float r, float g, float b);

	void GetColor(float& r, float& g, float& b);

	void Update(float dt);
	inline bool IsAlive() const { return _isAlive; }

	bool GetIsWithinLifetime() const;
	bool GetCanExplode() const {return _canExplode;}

	void Deactivate();
	void Activate();

	float GetMaxLifetime() const {return _maxLifetime;}
	float GetCurrLifetime() const { return _currLifetime; }

private:
	Vec2F _position;
	Vec2F _speedVec;
	float _speed = 0.f;
	bool _isAlive = false;
	bool _canExplode = false;
	float _currLifetime = 0.f;
	float _maxLifetime = 0.f;
	float _color[3] = {1.f, 1.f, 1.f};
};
