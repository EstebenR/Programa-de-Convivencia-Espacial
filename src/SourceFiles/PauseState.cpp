#include "PauseState.h"
#include "Texture.h"
#include "Resources.h"
#include "UIViewer.h"
#include "SDL_Game.h"
#include "InputBinder.h"
#include "GameStateMachine.h"
#include "MatchInfo.h"

void PauseState::init()
{
	offsetBetweenButtons_ = CONST(int, "PAUSE_OFFSET_BETWEEN_BUTTONS");
	maxMusicVolume_ = CONST(int, "MAX_MUSIC_VOLUME");
	maxEffectsVolume_ = CONST(int, "MAX_EFFECTS_VOLUME");
	currentMusicVolume_ = maxMusicVolume_;
	currentEffectsVolume_ = maxEffectsVolume_;

	entityManager_ = new EntityManager();

	Entity* pauseText = entityManager_->addEntity();
	Entity* resumeText = entityManager_->addEntity();
	Entity* musicText = entityManager_->addEntity();
	Entity* effectsText = entityManager_->addEntity();
	Entity* exitText = entityManager_->addEntity();
	Entity* musicSlider = entityManager_->addEntity();
	Entity* effectsSlider = entityManager_->addEntity();

	Entity* rocket = entityManager_->addEntity();
	Entity* musicSliderControl = entityManager_->addEntity();
	Entity* effectsSliderControl = entityManager_->addEntity();

	pauseText->addComponent<UIViewer>(Resources::PauseText, b2Vec2((CONST(int, "WINDOW_WIDTH") - SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::PauseText)->getFrameWidth() * 2.5) * 0.5f, CONST(int, "PAUSE_OFFSET_Y")), 2.5, 0);

	btns_.push_back(resumeText->addComponent<UIViewer>(Resources::ResumeText, b2Vec2((CONST(int, "WINDOW_WIDTH") - SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::ResumeText)->getFrameWidth() * 1.5) * 0.5f
		, (CONST(int, "WINDOW_HEIGHT") * 0.5f) + offsetBetweenButtons_ * -1), 1.5, 0));
	btns_.push_back(musicText->addComponent<UIViewer>(Resources::MusicText, b2Vec2((CONST(int, "WINDOW_WIDTH") * 0.5f - SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::MusicText)->getFrameWidth() * 1.5),
		(CONST(int, "WINDOW_HEIGHT") * 0.5f) + offsetBetweenButtons_ * 0), 1.5, 0));
	btns_.push_back(effectsText->addComponent<UIViewer>(Resources::EffectsText, b2Vec2((CONST(int, "WINDOW_WIDTH") * 0.5f - SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::EffectsText)->getFrameWidth() * 1.5),
		(CONST(int, "WINDOW_HEIGHT") * 0.5f) + offsetBetweenButtons_ * 1), 1.5, 0));
	btns_.push_back(exitText->addComponent<UIViewer>(Resources::MainMenuText, b2Vec2((CONST(int, "WINDOW_WIDTH") - SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::MainMenuText)->getFrameWidth() * 1.5) * 0.5f,
		(CONST(int, "WINDOW_HEIGHT") * 0.5f) + +offsetBetweenButtons_ * 2), 1.5, 0));

	buttonSelectorImage_ = rocket->addComponent<UIViewer>(Resources::Rocket, b2Vec2(btns_[selectedBtn_]->getPosUIElement().x - 160, btns_[selectedBtn_]->getPosUIElement().y - 30), 0.8, 90);

	musicSliderImage_ = musicSlider->addComponent<UIViewer>(Resources::Slider, b2Vec2(CONST(int, "WINDOW_WIDTH") * 0.5f, btns_[Buttons::Music]->getPosUIElement().y), 2, 0);
	effectsSliderImage_ = effectsSlider->addComponent<UIViewer>(Resources::Slider, b2Vec2(CONST(int, "WINDOW_WIDTH") * 0.5f, btns_[Buttons::Effects]->getPosUIElement().y), 2, 0);

	musicSliderControlImage_ = musicSliderControl->addComponent<UIViewer>(Resources::SliderControl, b2Vec2((musicSliderImage_->getPosUIElement().x + 33) + ((currentMusicVolume_ / 10) * (250 / (maxMusicVolume_ / 10))), btns_[Buttons::Music]->getPosUIElement().y), 2, 0);
	effectsSliderControlImage_ = effectsSliderControl->addComponent<UIViewer>(Resources::SliderControl, b2Vec2((effectsSliderImage_->getPosUIElement().x + 33) + ((currentEffectsVolume_ / 10) * (250 / (maxEffectsVolume_ / 10))), btns_[Buttons::Effects]->getPosUIElement().y), 2, 0);

	//cout << "Pause initialized to " << ownerPlayerID_ << endl;
	ownerBinder_ = SDL_Game::instance()->getStateMachine()->getMatchInfo()->getPlayersInfo()->at(ownerPlayerID_)->inputBinder;

	//FONDO
	fondo_ = SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::SpaceBackground);
}

void PauseState::render()
{
	fondo_->render(0, 0);
	GameState::render();
}

void PauseState::handleInput()
{
	GameState::handleInput();

	//Input espec�fico para el bot�n seleccionado
	switch (selectedBtn_)
	{
	case Buttons::Resume:
		if (ownerBinder_->menuForward())
			resumeGame();
		break;
	case Buttons::Music:
		if (handleSlider(currentMusicVolume_, maxMusicVolume_)) updateSoundVolume();
		break;
	case Buttons::Effects:
		if (handleSlider(currentEffectsVolume_, maxEffectsVolume_)) updateSoundVolume();
		break;
	case Buttons::Exit:
		if (ownerBinder_->menuForward()) {
			SDL_Game::instance()->getAudioMngr()->playChannel(Resources::MenuForward, 0);
			cout << "Pause ended for " << ownerPlayerID_ << endl;
			SDL_Game::instance()->getStateMachine()->changeToState(States::playableMenu);
			SDL_Game::instance()->getStateMachine()->deleteState(States::menu);
		}
		break;
	}

	//Input general para el men�
	if (ownerBinder_->pressPause()) {
		selectedBtn_ = 0;
		updateSelectedButton();
		resumeGame();
	}

	if (!holdingY_)
	{
		if (ownerBinder_->menuMove(Dir::Down)) {
			SDL_Game::instance()->getAudioMngr()->playChannel(Resources::MenuMove, 0);
			selectedBtn_++;
			updateSelectedButton();
			holdingY_ = true;
		}
		else if (ownerBinder_->menuMove(Dir::Up)) {
			SDL_Game::instance()->getAudioMngr()->playChannel(Resources::MenuMove, 0);
			selectedBtn_--;
			updateSelectedButton();
			holdingY_ = true;
		}
	}
	else if (holdingY_ && !ownerBinder_->menuMove(Dir::Up) && !ownerBinder_->menuMove(Dir::Down)) {
		holdingY_ = false;
	}
}

void PauseState::setOwner(int ownerID) {
	ownerPlayerID_ = ownerID;
	ownerBinder_ = SDL_Game::instance()->getStateMachine()->getMatchInfo()->getPlayersInfo()->at(ownerID)->inputBinder;
}

void PauseState::updateSelectedButton()
{
	if (selectedBtn_ >= 0) selectedBtn_ %= int(btns_.size());
	else {
		selectedBtn_ %= int(btns_.size());
		selectedBtn_ += int(btns_.size());
	}

	float newXPos = btns_[selectedBtn_]->getPosUIElement().x - 160;
	float newYPos = btns_[selectedBtn_]->getPosUIElement().y - 30;
	buttonSelectorImage_->setPosUIElement(b2Vec2(newXPos, newYPos));
}

void PauseState::updateSoundVolume()
{
	SDL_Game::instance()->getAudioMngr()->setMusicVolume(currentMusicVolume_);
	SDL_Game::instance()->getAudioMngr()->setChannelVolume(currentEffectsVolume_, 0);

	float newXPos = (musicSliderImage_->getPosUIElement().x + 33) + ((currentMusicVolume_ / 10) * (250 / (maxMusicVolume_ / 10)));
	musicSliderControlImage_->setPosUIElement(b2Vec2(newXPos, btns_[Buttons::Music]->getPosUIElement().y));

	newXPos = (effectsSliderImage_->getPosUIElement().x + 33) + ((currentEffectsVolume_ / 10) * (250 / (maxEffectsVolume_ / 10)));
	effectsSliderControlImage_->setPosUIElement(b2Vec2(newXPos, btns_[Buttons::Effects]->getPosUIElement().y));
}

bool PauseState::handleSlider(int& currentVolumeValue, const int& maxVolumeValue)
{
	if (!holdingX_)
	{
		if (currentVolumeValue > 0 && ownerBinder_->menuMove(Dir::Left)) {
			currentVolumeValue -= 10;
			holdingX_ = true;
		}
		else if (currentVolumeValue < maxVolumeValue && ownerBinder_->menuMove(Dir::Right)) {
			currentVolumeValue += 10;
			holdingX_ = true;
		}
	}
	else if (holdingX_ && !ownerBinder_->menuMove(Dir::Right) && !ownerBinder_->menuMove(Dir::Left)) {
		holdingX_ = false;
	}

	return holdingX_;
}

void PauseState::resumeGame()
{
	SDL_Game::instance()->getStateMachine()->changeToState(States::play);
	SDL_Game::instance()->getAudioMngr()->resumeMusic();
}