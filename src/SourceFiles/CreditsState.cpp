#include "CreditsState.h"
#include "Texture.h"
#include "Viewer.h"
#include "PlayerData.h"
#include "GameStateMachine.h"
#include "Component.h"

#include "PlayerFactory.h"
#include "UIViewer.h"


CreditsState::~CreditsState()
{
	delete entityManager_;		entityManager_ = nullptr;
}

void CreditsState::init()
{
	entityManager_ = new EntityManager();

	secondsPerFrame_ = CONST(double, "SECONDS_PER_FRAME");

	//FONDO
	fondo_ = SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::CreditsRoom);

	//M�SICA
	//SDL_Game::instance()->getAudioMngr()->playMusic(Resources::MainMenuMusic, -1);

	//Texto para vover al men�
	continueText = entityManager_->addEntity();
	continueText->addComponent<UIViewer>(Resources::ReturnText, b2Vec2((CONST(int, "WINDOW_WIDTH") * 0.5f) - 600, CONST(int, "WINDOW_HEIGHT") - 90), 1, 0);
}

void CreditsState::render()
{
	fondo_->render(0, 0);
	GameState::render();
}

void CreditsState::handleInput()
{
	GameState::handleInput();

	InputHandler* ih = SDL_Game::instance()->getInputHandler();
	if (ih->keyDownEvent() || ih->isButonDownEvent()) {
		GameStateMachine* gsMachine = SDL_Game::instance()->getStateMachine();
		gsMachine->transitionToState(States::playableMenu);
	}
}