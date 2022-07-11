#include "AbstractTimedGameMode.h"
#include "PlayState.h"

void AbstractTimedGameMode::init(PlayState* game)
{
	GameMode::init(game);
	sPerFrame_ = CONST(double, "SECONDS_PER_FRAME");
	suddenDeathRenderTime_ = CONST(double, "SUDDEN_DEATH_RENDER_TIME");

	winWidth_ = CONST(int, "WINDOW_WIDTH");
	winHeigth_ = CONST(int, "WINDOW_HEIGHT");

	halfWinWidth_ = winWidth_ * 0.5;
	halfWinHeight_ = winHeigth_ * 0.5;
	canvasTimerTexture_ = SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::CanvasTimerBackground);
	canvasTimerRect_.x = halfWinWidth_ - CONST(int, "COUNTDOWN_UI_OFFSET_X");
	canvasTimerRect_.y = 0;
	canvasTimerRect_.w = CONST(int, "COUNTDOWN_UI_WIDTH");
	canvasTimerRect_.h = CONST(int, "COUNTDOWN_UI_HEIGTH");
}

void AbstractTimedGameMode::render()
{
	int minutes = 0, seconds = 0;

	if (timeToEnd_ < timeSinceStart_) {
		if (roundFinished_ && winnerId_ != -1) {
			Texture* ganador = SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::winner1 + winnerId_);
			SDL_Rect destRect{
				halfWinWidth_ - (int)(ganador->getWidth() * 0.5),
				halfWinHeight_,
				ganador->getWidth(),
				ganador->getHeight()
			};
			ganador->render(destRect, 0, 0);
		}
		else {
			suddenDeathRenderTimer_ += sPerFrame_;
			if (suddenDeathRenderTimer_ >= suddenDeathRenderTime_) {
				suddenDeathRendering_ = !suddenDeathRendering_;
				suddenDeathRenderTimer_ = 0;
			}
			if (suddenDeathRendering_) {
				SDL_Rect suddenDeathRect;
				suddenDeathRect.x = halfWinWidth_ - suddenDeathTexture_->getWidth() * 1.5;
				suddenDeathRect.y = halfWinHeight_ - suddenDeathTexture_->getHeight() * 1.5;
				suddenDeathRect.w = suddenDeathTexture_->getWidth() * 3;
				suddenDeathRect.h = suddenDeathTexture_->getHeight() * 3;
				suddenDeathTexture_->render(suddenDeathRect, 0, 0);
			}
		}
	}
	else {
		minutes = (timeToEnd_ - timeSinceStart_) * minutesFreq_;
		seconds = (int)(timeToEnd_ - timeSinceStart_) % 60;
	}
	renderTimer(seconds, minutes);
}

void AbstractTimedGameMode::renderTimer(int seconds, int minutes)
{
	canvasTimerTexture_->render(canvasTimerRect_, 0, 0);

	string timeString = to_string(minutes);
	vector<Texture*> timeTextTextures;

	for (int i = 0; i < timeString.length(); i++) { //sacamos los d�gitos y los metemos en un vector
		timeTextTextures.push_back(SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::Zero + timeString[i] - '0'));
	}
	timeTextTextures.push_back(SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::Colon));
	timeString = to_string(seconds);
	if (timeString.length() < 2) timeTextTextures.push_back(SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::Zero)); //forzamos 2 digitos
	for (int i = 0; i < timeString.length(); i++) {
		timeTextTextures.push_back(SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::Zero + timeString[i] - '0'));
	}

	for (int i = 0; i < timeTextTextures.size(); i++) {
		SDL_Rect timeTextRect;
		timeTextRect.x = halfWinWidth_ - timeTextTextures[i]->getWidth() * 0.33333 * timeTextTextures.size() + timeTextTextures[i]->getWidth() * 0.666666 * i;
		timeTextRect.y = 7;
		timeTextRect.w = timeTextTextures[i]->getWidth() * 0.7;
		timeTextRect.h = timeTextTextures[i]->getHeight() * 0.7;

		timeTextTextures[i]->render(timeTextRect, 0, 0);
	}
}

void AbstractTimedGameMode::update()
{
	GameMode::update();
}

void AbstractTimedGameMode::activateControl()
{
	setActiveTimer(true);
	GameMode::activateControl();
}

void AbstractTimedGameMode::updateTime(const vector<int>& playerPoints)
{
	if (activeTimer_) {
		timeSinceStart_ += sPerFrame_;
		if (timeSinceStart_ >= timeToEnd_ && !roundFinished_) {
			int maxPoints = 0;
			bool suddenDeath = false;
			for (int i = 0; i < playerPoints.size(); i++) {
				if (playerPoints[i] > maxPoints) {
					maxPoints = playerPoints[i];
					winnerId_ = i;
					suddenDeath = false;
				}
				else if (playerPoints[i] == maxPoints) {
					suddenDeath = true;
				}
			}
			if (!suddenDeath) {
				roundFinished_ = true;
				cout << "PLAYER: " << winnerId_ << " WON." << endl;
			}
		}
	}
}