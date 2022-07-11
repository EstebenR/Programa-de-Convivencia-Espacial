#pragma once
#include "GameMode.h"
#include "Resources.h"
#include "Texture.h"
#include "FireBallGenerator.h"

class Countdown : public Component
{
private:
	int currentFrame_ = 0;
	int maxFrames_ = CONST(int, "MAX_COUNTDOWN_FRAMES");
	int framesPerNum_ = maxFrames_ / 4;

	int numAct_ = 0;

	int minSize_ = CONST(int, "MIN_COUNTDOWN_SIZE");
	int maxSize_ = CONST(int, "MAX_COUNTDOWN_SIZE");
	int currentSize_ = 0;
	int sizeChangeRate_ = (maxSize_ - minSize_) / framesPerNum_; //ratio de crecimiento teniendo en cuenta que hay 4 texturas

	int posX_ = CONST(int, "WINDOW_WIDTH") * 0.5f;
	int posY_ = CONST(int, "WINDOW_HEIGHT") * 0.5f;

	Texture* textures_[4] = { //3 2 1 go
	SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::Three),
	SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::Two),
	SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::One),
	SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::Go) };

	Texture* currentNumTexture_ = textures_[0];

	GameMode* gMode_;
	FireBallGenerator* fGen_;

public:
	Countdown(GameMode* g) : Component(ComponentType::Countdown), gMode_(g) {}
	virtual ~Countdown() {}
	virtual void update();
	virtual void draw() const;
	void assignBoiler(FireBallGenerator* fGen) { fGen_ = fGen; }
};
