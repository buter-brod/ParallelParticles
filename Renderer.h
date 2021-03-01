#pragma once

#include <thread>
#include <vector>

struct GLFWwindow;
class Effect;
class Particle;
class ParticleSystem;

class Renderer
{
public:

	Renderer(ParticleSystem* system);
	bool SetSize(int x, int y);
	void Stop();

protected:
	
	bool init();
	void loop();

	void render();
	void beginRender();
	void endRender();
	void updateFPS();

	std::vector<Effect>& getEffects();
	void renderEffect(Effect&);
	void renderParticle(Particle&);

private:
	unsigned int _fShader = 0;
	unsigned int _vShader = 0;
	unsigned int _shader = 0;

	unsigned int _vao = 0;
	unsigned int _vbo = 0;

	GLFWwindow* _window = nullptr;

	int _width = 0;
	int _height = 0;

	int _frameCount = 0;
	double _previousFPSTime = 0.0;

	double _prevRenderTime = 0.f;

	ParticleSystem* _particleSystem = nullptr;

	bool _stopRequest = false;

	unsigned _effectsRendered = 0;
	unsigned _particlesRendered = 0;
};

