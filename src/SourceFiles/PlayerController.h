#pragma once
#include "Component.h"
#include "box2d.h"
#include "Collider.h"
#include "AttachesToObjects.h"
#include "PlayerData.h"
#include "checkML.h"
#include "InputBinder.h"
#include "ParticleEmitter.h"

class AnimatedPlayer;

class PlayerController : public Component
{
private:
	bool chargingImpulse_ = false;
	Uint16 chargedFrames_;
	float impulseForce_;
	float chargeMultiplier_;
	float maxImpulseGrabbed_;
	float maxImpulseFloating_;
	float maxSpeedAfterImpulse_;
	float impulseRadError_; //el angulo de impulso permitio es PI + 2*impulseRadError

	int playerHeight_;
	int minFartForce_;

	int playerNumber_;
	b2Vec2 dirImpulse_;

	Collider* coll_ = nullptr;
	AttachesToObjects* attachesToObj_ = nullptr;
	PlayerData* playerData_ = nullptr;
	InputBinder* ib = nullptr;
	AnimatedPlayer* viewer_ = nullptr;
	ParticleEmitter* emitter_ = nullptr;

	KeyboardBinder* kBinder_ = nullptr;

	int impulseCooldown_ = 0;
	int impulseCooldownTimer_ = 0;

	bool isImpulseValid(const b2Vec2& dir); //prevents the player from impulsing against the body is attached to
public:
	PlayerController();
	virtual ~PlayerController() { };

	void init() override;
	void handleInput() override;
	void update() override;

	float getImpulseForce() { return impulseForce_; };
	void resetImpulseForce() { impulseForce_ = 0;  chargedFrames_ = 0; };
	bool isChargingImpulse() { return chargingImpulse_; };
	bool getImpulseValid();
};
