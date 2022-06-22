#pragma once
static constexpr double particleMinLifetime = 1.f;
static constexpr double particleMaxLifetime = 3.f;

static constexpr float particleMinSpeed = 0.1f;
static constexpr float particleMaxSpeed = 0.3f;

static constexpr float particleScaleDefault = 0.01f;
static constexpr double particleFadeoutTime = 0.5;
static constexpr float particleAlpha = 0.75f;

static constexpr float particleExplodeProbability = 0.25f;

static constexpr unsigned int maxEffectsCount = 128;
static constexpr double particleSystemTimeStep = 0.03;

static constexpr unsigned int maxParticlesPerEffectCount = 512;
static constexpr double effectSimTimeStep = 0.02;

static constexpr float particleSystemTimeScale = 1.f;
static constexpr float effectSimTimeScale = 1.f;

static constexpr int sceneWidth = 1024;
static constexpr int sceneHeight = 768;

//static constexpr unsigned maxThreads = 1024;