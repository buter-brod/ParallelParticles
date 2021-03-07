#pragma once
static constexpr float particleMinLifetime = 1.f;
static constexpr float particleMaxLifetime = 3.f;

static constexpr float particleMinSpeed = 0.1f;
static constexpr float particleMaxSpeed = 0.3f;

static constexpr float particleScaleDefault = 0.01f;
static constexpr float particleFadeoutTime = 0.5f;
static constexpr float particleAlpha = 0.75f;

static constexpr float particleExplodeProbability = 0.125f;

static constexpr unsigned int maxEffectsCount = 128;
static constexpr float particleSystemTimeStep = 0.01f;

static constexpr unsigned int maxParticlesPerEffectCount = 128;
static constexpr float effectSimTimeStep = 0.01f;

static constexpr float particleSystemTimeScale = 1.f;
static constexpr float effectSimTimeScale = 1.f;

static constexpr int sceneWidth = 1024;
static constexpr int sceneHeight = 768;