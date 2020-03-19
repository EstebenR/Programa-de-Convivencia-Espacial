#pragma once
#include "Component.h"
#include "box2d.h"
#include "Collider.h"
#include "AttachesToObjects.h"



class PlayerController : public Component
{
private:
	b2Vec2 dirImpulse_;
	Collider* coll_;
	AttachesToObjects* attachesToObj_;
	float impulseForce_;
	int playerNumber_;//N�mero del jugador (de 1 a 4));
public:
	PlayerController(int p);
	~PlayerController() {}

	void init() override;
	void handleInput() override;

	void setForce(float f) { impulseForce_ = f; }
};
