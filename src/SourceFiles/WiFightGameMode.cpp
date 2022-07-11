#include "WiFightGameMode.h"
#include "Resources.h"
#include "ParticleEmitter.h"
#include "RouterLogic.h"
#include "ColliderViewer.h"

void WiFightGameMode::init(PlayState* game)
{
	GameMode::init(game);

	halfWinWidth_ = CONST(int, "WINDOW_WIDTH") * 0.5f;
	halfWinHeight_ = CONST(int, "WINDOW_HEIGHT") * 0.5f;
	pointsToWin_ = CONST(double, "POINTS_TO_WIN");

	GameMode::createPlayers(game);
	playerProgress_.reserve(nPlayers_);
	for (int k = 0; k < nPlayers_; k++) {
		playerProgress_.push_back(0);
	}

	Entity* router = ObjectFactory::makeRouter(state_->getEntityManager(), state_->getPhysicsWorld(), tilemap_->getObjSpecialSpawnPos(), this, &wifiWavesPool_);

	wifiWavesPool_.init(state_->getEntityManager(), state_->getPhysicsWorld(), router->getComponent<Collider>(ComponentType::Collider));

	GameMode::initProgressBars();
}

void WiFightGameMode::render()
{
	GameMode::renderProgressBars(playerProgress_, pointsToWin_);

	if (roundFinished_) {
		Texture* ganador = SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::winner1 + winnerId_);
		SDL_Rect destRect{
			halfWinWidth_ - (int)(ganador->getWidth() * 0.5f),
			halfWinHeight_,
			ganador->getWidth(),
			ganador->getHeight()
		};
		ganador->render(destRect, 0, 0);
	}
}

void WiFightGameMode::addPoints(int player, double sumPoints)
{
	if (!roundFinished_) {
		playerProgress_[player] += sumPoints;
		if (playerProgress_[player] >= pointsToWin_) {
			cout << "Player " << player << " won!" << endl;
			winnerId_ = player;
			roundFinished_ = true;
		}
	}
}