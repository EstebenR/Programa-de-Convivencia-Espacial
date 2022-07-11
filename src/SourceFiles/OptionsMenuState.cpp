#include "OptionsMenuState.h"
#include "Texture.h"
#include "Resources.h"
#include "UIViewer.h"
#include "SDL_Game.h"
#include "InputBinder.h"
#include "GameStateMachine.h"

OptionsMenuState::~OptionsMenuState()
{
	delete(ownerBinder_);     ownerBinder_ = nullptr;
}

void OptionsMenuState::init()
{
	offsetBetweenButtons_ = CONST(int, "PAUSE_OFFSET_BETWEEN_BUTTONS");
	maxMusicVolume_ = CONST(int, "MAX_MUSIC_VOLUME");
	maxEffectsVolume_ = CONST(int, "MAX_EFFECTS_VOLUME");
	currentMusicVolume_ = maxMusicVolume_;
	currentEffectsVolume_ = maxEffectsVolume_;

	entityManager_ = new EntityManager();

	//Textos
	Entity* OptionsText = entityManager_->addEntity();
	Entity* musicText = entityManager_->addEntity();
	Entity* effectsText = entityManager_->addEntity();
	Entity* GoBack = entityManager_->addEntity();

	//Sliders
	Entity* musicSlider = entityManager_->addEntity();
	Entity* effectsSlider = entityManager_->addEntity();

	//Punteros e indicadores
	Entity* rocket = entityManager_->addEntity();
	Entity* musicSliderControl = entityManager_->addEntity();
	Entity* effectsSliderControl = entityManager_->addEntity();

	OptionsText->addComponent<UIViewer>(Resources::OptionsText, b2Vec2((CONST(int, "WINDOW_WIDTH") * 0.5f) - 340, (CONST(int, "WINDOW_HEIGHT") * 0.5f) - 310), 2.5, 0);

	btns_.push_back(musicText->addComponent<UIViewer>(Resources::MusicText, b2Vec2((CONST(int, "WINDOW_WIDTH") * 0.5f - SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::MusicText)->getFrameWidth() * 1.5),
		(CONST(int, "WINDOW_HEIGHT") * 0.5f) + offsetBetweenButtons_ * 0), 1.5, 0));
	btns_.push_back(effectsText->addComponent<UIViewer>(Resources::EffectsText, b2Vec2((CONST(int, "WINDOW_WIDTH") * 0.5f - SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::EffectsText)->getFrameWidth() * 1.5),
		(CONST(int, "WINDOW_HEIGHT") * 0.5f) + offsetBetweenButtons_ * 1), 1.5, 0));
	btns_.push_back(GoBack->addComponent<UIViewer>(Resources::GoBackText, b2Vec2(((CONST(int, "WINDOW_WIDTH") - SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::GoBackText)->getFrameWidth() * 1.5) * 0.5f), (CONST(int, "WINDOW_HEIGHT") * 0.5f) + +offsetBetweenButtons_ * 2), 1.5, 0));

	buttonSelectorImage_ = rocket->addComponent<UIViewer>(Resources::Rocket, b2Vec2(btns_[selectedBtn_]->getPosUIElement().x - 160, btns_[selectedBtn_]->getPosUIElement().y - 30), 0.8, 90);

	musicSliderImage_ = musicSlider->addComponent<UIViewer>(Resources::Slider, b2Vec2(CONST(int, "WINDOW_WIDTH") * 0.5f, btns_[Button::Music]->getPosUIElement().y), 2, 0);
	effectsSliderImage_ = effectsSlider->addComponent<UIViewer>(Resources::Slider, b2Vec2(CONST(int, "WINDOW_WIDTH") * 0.5f, btns_[Button::Effects]->getPosUIElement().y), 2, 0);

	musicSliderControlImage_ = musicSliderControl->addComponent<UIViewer>(Resources::SliderControl, b2Vec2((musicSliderImage_->getPosUIElement().x + 33) + ((currentMusicVolume_ / 10) * (250 / (maxMusicVolume_ / 10))), btns_[Button::Music]->getPosUIElement().y), 2, 0);
	effectsSliderControlImage_ = effectsSliderControl->addComponent<UIViewer>(Resources::SliderControl, b2Vec2((effectsSliderImage_->getPosUIElement().x + 33) + ((currentEffectsVolume_ / 10) * (250 / (maxEffectsVolume_ / 10))), btns_[Button::Effects]->getPosUIElement().y), 2, 0);

	if (SDL_Game::instance()->getInputHandler()->getNumControllers() > 0) {
		ownerBinder_ = new ControllerBinder(0);
	}
	else {
		ownerBinder_ = new PureKeyboardBinder(1);
	}
	//FONDO
	fondo_ = SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::SpaceBackground);
}

void OptionsMenuState::render()
{
	fondo_->render(0, 0);
	GameState::render();
}

void OptionsMenuState::handleInput()
{
	GameState::handleInput();

	//Input espec�fico para el bot�n seleccionado
	switch (selectedBtn_)
	{/*
	case Button::Resume:
		if (ownerBinder_->menuForward())
			resumeGame();
		break;*/
	case Button::Music:
		if (handleSlider(currentMusicVolume_, maxMusicVolume_)) updateSoundVolume();
		break;
	case Button::Effects:
		if (handleSlider(currentEffectsVolume_, maxEffectsVolume_)) updateSoundVolume();
		break;
	case Button::Exit:
		if (ownerBinder_->menuForward()) {
			SDL_Game::instance()->getAudioMngr()->playChannel(Resources::MenuForward, 0);
			//cout << "Pause ended for " << ownerPlayerID_ << endl;
			SDL_Game::instance()->getStateMachine()->transitionToState(States::playableMenu);
		}
		break;
	}

	//Input general para el men�
	if (ownerBinder_->pressPause()) {
		selectedBtn_ = 0;
		updateSelectedButton();
		SDL_Game::instance()->getStateMachine()->transitionToState(States::playableMenu);
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

void OptionsMenuState::updateSelectedButton()
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

void OptionsMenuState::updateSoundVolume()
{
	SDL_Game::instance()->getAudioMngr()->setMusicVolume(currentMusicVolume_);
	SDL_Game::instance()->getAudioMngr()->setChannelVolume(currentEffectsVolume_, 0);

	float newXPos = (musicSliderImage_->getPosUIElement().x + 33) + ((currentMusicVolume_ / 10) * (250 / (maxMusicVolume_ / 10)));
	musicSliderControlImage_->setPosUIElement(b2Vec2(newXPos, btns_[Button::Music]->getPosUIElement().y));

	newXPos = (effectsSliderImage_->getPosUIElement().x + 33) + ((currentEffectsVolume_ / 10) * (250 / (maxEffectsVolume_ / 10)));
	effectsSliderControlImage_->setPosUIElement(b2Vec2(newXPos, btns_[Button::Effects]->getPosUIElement().y));
}

bool OptionsMenuState::handleSlider(int& currentVolumeValue, const int& maxVolumeValue)
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