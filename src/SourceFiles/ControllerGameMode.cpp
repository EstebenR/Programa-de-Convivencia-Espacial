#include "ControllerGameMode.h"
#include <map>

ControllerGameMode::~ControllerGameMode()
{
}

void ControllerGameMode::init(PlayState* game) {
	GameMode::init(game);
	GameMode::createPlayers(game);
	controller_ = ObjectFactory::makeController(state_->getEntityManager(), state_->getPhysicsWorld(), b2Vec2(tilemap_->getObjSpecialSpawnPos().x,
		tilemap_->getObjSpecialSpawnPos().y), b2Vec2(CONST(float, "CONTROLLER_W_PHYSICS"), CONST(float, "CONTROLLER_H_PHYSICS")));
	for (Entity* player : players_) controllerTimes_.push_back(0);

	GameMode::initProgressBars();
}

void ControllerGameMode::update() {
	if (!roundFinished_) {
		if (controller_->IsPicked()) {
			controllerTimes_[controller_->getPlayerId()] += msPerFrame_;
			if (controllerTimes_[controller_->getPlayerId()] >= timeToWin_) {
				roundFinished_ = true;
				winnerId_ = controller_->getPlayerId();
			}
		}
	}
	GameMode::update();
}

void ControllerGameMode::render() {
	GameMode::renderProgressBars(controllerTimes_, timeToWin_);

	if (roundFinished_) {
		Texture* ganador = SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::winner1 + winnerId_);
		SDL_Rect destRect{
			halfWidth_ - (int)(ganador->getWidth() * 0.5),
			halfHeight_,
			ganador->getWidth(),
			ganador->getHeight()
		};
		ganador->render(destRect, 0, 0);
	}
}