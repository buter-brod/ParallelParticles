#include "Renderer.h"
#include <cstdio>
#include <functional>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Config.h"
#include "ParticleSystem.h"
#include "Utils.h"

constexpr float fpsUpdateInterval = 0.25f;

Renderer*& RendererPtr() {
	static Renderer* r = nullptr;
	return r;
}

void windowCloseCB(GLFWwindow* window) {
	auto* renderer = RendererPtr();
	renderer->Stop();
}

void windowSizeCB(GLFWwindow* window, int x, int y)
{
	auto* renderer = RendererPtr();
	if (renderer)
		renderer->SetSize(x, y);
}

Renderer::Renderer(ParticleSystem* system) : _particleSystem(system) {
	init();
}

void Renderer::Stop() {
	_stopRequest = true;
}

bool Renderer::SetSize(const int x, const int y) {

	const bool changed = _width != x || _height != y;

	_width = x;
	_height = y;

	return changed;
}

void Renderer::updateFPS()
{
	if (_previousFPSTime == 0.0)
		_previousFPSTime = getTime();

	const auto currentTime = getTime();
	const auto elapsed = currentTime - _previousFPSTime;
	if (elapsed > fpsUpdateInterval)
	{
		_previousFPSTime = currentTime;
		const double fps = static_cast<double>(_frameCount) / elapsed;
		char txtBuf[128];
		sprintf_s(txtBuf, "opengl @ fps: %.2f, particles %i effects %i", fps, _particlesRendered, _effectsRendered);
		glfwSetWindowTitle(_window, txtBuf);
		_frameCount = 0;
	}

	++_frameCount;
}

const char* getVerterShader() {
	
	static const char* shaderStr =
		"#version 400\n"
		"in vec3 vp;"
		"uniform float scale;"
		"uniform vec2 offset;"
		"out vec3 color;"
		"void main() {"
		"vec4 position = vec4(vp, 1.0);"
		"position[0] = position[0] * scale + offset[0];"
		"position[1] = position[1] * scale + offset[1];"
		"gl_Position = position;"
		"}";

	return shaderStr;
}

const char* getFragmentShader()
{
	static const char* fragmentStr =
		"#version 400\n"
		"uniform float alpha;"
		"uniform vec3 color;"
		"out vec4 frag_color;"
		"void main() {"
		"frag_color = vec4(color, alpha);"
		"}";

	return fragmentStr;
}

bool Renderer::init() {

	RendererPtr() = this;

	if (!glfwInit())
	{
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	SetSize(sceneWidth, sceneHeight);

	_window = glfwCreateWindow(_width, _height, "ParallelParticles", nullptr, nullptr);

	if (!_window)
	{
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(_window);
	glewInit();
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	

	float points[] = {
		0.0f,  1.f,  0.0f,
		1.f, -1.f,  0.0f,
		-1.f, -1.f,  0.0f
	};

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	const GLuint vs = glCreateShader(GL_VERTEX_SHADER);

	const auto* vsStr = getVerterShader();
	const auto* fsStr = getFragmentShader();

	glShaderSource(vs, 1, &vsStr, nullptr);
	glCompileShader(vs);

	const GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fsStr, nullptr);
	glCompileShader(fs);

	//{
	//	GLint isCompiled = 0;
	//	glGetShaderiv(vs, GL_COMPILE_STATUS, &isCompiled);
	//	if (isCompiled == GL_FALSE)
	//	{
	//		GLint maxLength = 0;
	//		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &maxLength);

	//		// The maxLength includes the NULL character
	//		std::vector<GLchar> errorLog(maxLength);
	//		glGetShaderInfoLog(vs, maxLength, &maxLength, &errorLog[0]);
	//		char *j = new char [maxLength];
	//		memcpy(j, &errorLog[0], maxLength);

	//		// Provide the infolog in whatever manor you deem best.
	//		// Exit with failure.
	//		glDeleteShader(vs); // Don't leak the shader.
	//	}
	//}

	_shader = glCreateProgram();
	glAttachShader(_shader, fs);
	glAttachShader(_shader, vs);
	glLinkProgram(_shader);

	glfwSetWindowSizeCallback(_window, windowSizeCB);
	glfwSetWindowCloseCallback(_window, windowCloseCB);

	initUniforms();

	loop();
	return true;
}

void Renderer::loop() {
	
	_prevRenderTime = getTime();

	while(!_stopRequest) {

		const auto currTime = getTime();
		const auto dt = static_cast<float>(currTime - _prevRenderTime);
	
		_particleSystem->Update(dt * particleSystemTimeScale);
		_prevRenderTime = getTime();

		render();
	}

	_particleSystem->Stop();

	glfwSetWindowShouldClose(_window, 1);
}

void Renderer::initUniforms() {

	const auto initUniform = [this](const std::string& name, int& var) {
		if (var < 0)
			var = glGetUniformLocation(_shader, name.c_str());
	};

	initUniform("alpha", _alphaUniform);
	initUniform("color", _colorUniform);
	initUniform("scale", _scaleUniform);
	initUniform("offset", _offsetUniform);
}

void Renderer::renderParticle(const ParticleVisualInfo& particleInfo) {

	glUniform1f(_alphaUniform, particleAlpha);
	
	const float r = particleInfo._color[0];
	const float g = particleInfo._color[1];
	const float b = particleInfo._color[2];
	
	glUniform3f(_colorUniform, r, g, b);

	float scale = particleScaleDefault;
	constexpr float zeroScale = 0.001f;

	if (!particleInfo.GetIsWithinLifetime()) {
		scale = zeroScale;
	}
	else 
	{
		const float maxTime = particleInfo._maxLifetime;
		const float currTime = particleInfo._currLifetime;
		const float remainingTime = maxTime - currTime;
		if (remainingTime < particleFadeoutTime)
		{
			const float coeff = std::max(remainingTime / particleFadeoutTime, zeroScale);
			scale *= coeff;
		}
	}
	
	const auto& pos = particleInfo._position;
	const float offsetX = 2.f * (pos._x - 0.5f);
	const float offsetY = 2.f * (pos._y - 0.5f);

	glUniform1f(_scaleUniform, scale);
	glUniform2f(_offsetUniform, offsetX, offsetY);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	++_particlesRendered;
}

void Renderer::renderEffect(const Effect& effect) {
	
	const auto& particles = effect.GetParticlesInfo();
	for (const auto& particleInfo : particles) {

		renderParticle(particleInfo);
	}

	++_effectsRendered;
}

void Renderer::render() {

	//printf("render\n");
	
	beginRender();

	auto& effects = _particleSystem->GetEffects();
	for (Effect& effect : effects)
	{
		if (effect.IsAlive())
			renderEffect(effect);
	}

	endRender();
}

void Renderer::endRender() {

	glfwSwapBuffers(_window);

	glfwPollEvents();
	if (GLFW_PRESS == glfwGetKey(_window, GLFW_KEY_ESCAPE))
		_particleSystem->SoftStop();
}

void Renderer::beginRender()
{
	updateFPS();

	_particlesRendered = 0;
	_effectsRendered = 0;
	
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, _width, _height);

	glUseProgram(_shader);
	glBindVertexArray(_vao);
}