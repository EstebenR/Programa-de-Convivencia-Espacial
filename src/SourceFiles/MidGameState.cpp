#include "MidGameState.h"
#include "Collider.h"
#include "Resources.h"
#include "AnimatedViewer.h"
#include "AnimatedUIViewer.h"
#include "Entity.h"
#include "SDL_Game.h"
#include "InputHandler.h"
#include "MatchInfo.h"


MidGameState::~MidGameState()
{
	delete entityManager_;		entityManager_ = nullptr;
}

void MidGameState::init()
{
	totalRounds_ = SDL_Game::instance()->getStateMachine()->getMatchInfo()->getNumberOfRounds();

	entityManager_ = new EntityManager();

	//Fondo del modo
	fondo = SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::RocketRoom);

	distanceGainedByPoint_ = (CONST(int, "WINDOW_WIDTH") - CONST(int, "START_POSITION")) / totalRounds_;

	markerScale_ = CONST(double, "MARKER_UI_SCALE");
	markerNumberScale_ = CONST(double, "MARKER_NUMBER_UI_SCALE");
	ZeroTex = Resources::NumZero;
	//creamos un marcador y un numero de ejemplo para no tener que calcular sus dimensiones todas las iteraciones
	Entity* sampleMarker = new Entity();
	UIViewer* sampleMarkerViewer = sampleMarker->addComponent<UIViewer>(Resources::Token, b2Vec2(0, 0), markerScale_, 0);
	UIViewer* sampleNumberViewer = sampleMarker->addComponent<UIViewer>((Resources::TextureId)ZeroTex, b2Vec2(0, 0), markerNumberScale_, 0);

	b2Vec2 markerSize = sampleMarkerViewer->getWHUIElement();
	b2Vec2 digitSize = sampleNumberViewer->getWHUIElement();
	delete sampleMarker;

	//Marcadores de las rondas
	for (int i = 0; i < totalRounds_; i++) {
		Entity* marker = entityManager_->addEntity(4);
		b2Vec2 markerPos = b2Vec2((CONST(int, "START_POSITION") + distanceGainedByPoint_ * i) - CONST(int, "MARKER_WIDTH"), CONST(int, "MARKER_Y_POSITION"));
		UIViewer* markerViewer = marker->addComponent<UIViewer>(Resources::Token, markerPos, markerScale_, 0);
		//hay que calcular el centro del marcador y luego buscar donde quedaria la esquina superior izquierda de la textura socorro
		b2Vec2 numberPos = b2Vec2(markerPos.x + markerSize.x * 0.5f - digitSize.x * 0.5f, markerPos.y + markerSize.y * 0.5f * 1.1 - digitSize.y * 0.5f);
		if (i < 10) {
			marker->addComponent<UIViewer>((Resources::TextureId)ZeroTex + i, numberPos, markerNumberScale_, 0);
		}
		else {
			marker->addComponent<UIViewer>((Resources::TextureId)ZeroTex + i / 10, b2Vec2(numberPos.x - digitSize.x * 0.5f, numberPos.y), markerNumberScale_, 0);
			marker->addComponent<UIViewer>((Resources::TextureId)ZeroTex + i % 10, b2Vec2(numberPos.x + digitSize.x * 0.5f, numberPos.y), markerNumberScale_, 0);
		}
		markers.push_back(marker);
	}

	int initPosY = (CONST(int, "WINDOW_HEIGHT") * 0.5f) - (-distanceBetweenRockets_ * 0.5f + (distanceBetweenRockets_ * 0.5f * numPlayers_));

	SDL_Rect rocketRect;
	rocketRect.x = rocketRect.y = 0;
	rocketRect.w = CONST(int, "ROCKET_W");
	rocketRect.h = CONST(int, "ROCKET_H");

	SDL_Rect stationRect;
	stationRect.x = 0;
	stationRect.y = 0;
	stationRect.w = CONST(int, "STATION_W");
	stationRect.h = CONST(int, "STATION_H");

	Entity* spaceStation = entityManager_->addEntity(2);
	spaceStationViewer_ = spaceStation->addComponent<AnimatedUIViewer>(Resources::SpaceStation, 3, b2Vec2(CONST(int, "WINDOW_WIDTH") * 0.5f - CONST(int, "SPACE_STATION_WIDTH") * 0.5f, CONST(int, "WINDOW_HEIGHT") * 0.5f - CONST(int, "SPACE_STATION_HEIGHT") * 0.5f), 1, 0);

	for (int k = 0; k < numPlayers_; k++) {
		Entity* newRocket = entityManager_->addEntity();
		//We could move the rockets using only the Viewer, but this will make
		//the logic much much easier.

		AnimatedUIViewer* viewer = newRocket->addComponent<AnimatedUIViewer>(Resources::RocketSpriteSheet, 10, b2Vec2(CONST(int, "START_ROCKET_POSITION"), (initPosY - CONST(int, "ROCKET_H")) + (distanceBetweenRockets_ * k) + rocketRect.h * 0.5f), 1, 0);
		playerRockets_.push_back(viewer);

		viewer->setFrame(0, k);	// cuando haya skins, descomentar esta línea y quitar startAnimation
		viewer->startAnimation();
	}
	//Texto para terminar la intermision
	continueText = entityManager_->addEntity();
	UIViewer* uiV = continueText->addComponent<UIViewer>(Resources::ContinueText, b2Vec2(0, 0), 1, 0);
	uiV->setPosUIElement(b2Vec2(((CONST(int, "WINDOW_WIDTH") - uiV->getW()) * 0.5f), CONST(int, "WINDOW_HEIGHT") - CONST(int, "UI_PRESS_KEY_OFFSET_Y")));
	continueText->setActive(false);
}

void MidGameState::render()
{
	fondo->render(0, 0);
	GameState::render();
}

void MidGameState::update()
{
	GameState::update();
	currentFrame++;

	if (currentFrame >= framesUntilNextShake_) {
		if (currentState != zoom) {
			int randDirX = 1, randDirY = 1;
			if (rand() % 2 == 0) randDirX = -1;
			if (rand() % 2 == 0) randDirY = -1;

			playerRockets_[roundWinner_]->setOffset(b2Vec2(rand() % shakeOffsetDistance_ * randDirX, rand() % shakeOffsetDistance_ * randDirY));
		}

		else {
			for (int k = 0; k < playerRockets_.size(); k++) {
				int randDirX = 1, randDirY = 1;
				if (rand() % 2 == 0) randDirX = -1;
				if (rand() % 2 == 0) randDirY = -1;

				playerRockets_[k]->setOffset(b2Vec2(rand() % shakeOffsetDistance_ * randDirX, rand() % shakeOffsetDistance_ * randDirY));
			}
		}

		framesUntilNextShake_ = currentFrame + CONST(int, "FRAMES_PER_SHAKE");
	}

	switch (currentState) {
	case animationState::waiting:
		if (currentFrame >= CONST(int, "WAITING_DURATION")) {
			currentState = movingWinner;
			rocketXPositionObjective_ = playerRockets_[roundWinner_]->getPosUIElement().x + distanceGainedByPoint_;
		}
		break;

	case animationState::movingWinner:
		playerRockets_[roundWinner_]->setPosUIElement(b2Vec2(playerRockets_[roundWinner_]->getPosUIElement().x + CONST(int, "ROCKET_VEL"), playerRockets_[roundWinner_]->getPosUIElement().y));

		if (playerRockets_[roundWinner_]->getPosUIElement().x > rocketXPositionObjective_) {
			currentState = waitingButton;
			continueText->setActive(true);
		}
		break;

	case animationState::waitingButton:
		if (buttonPush) {
			currentState = zoom;
			frameZoomStateEnds_ = currentFrame + CONST(int, "ZOOM_DURATION");

			for (int i = 0; i < markers.size(); i++) {
				markers[i]->setActive(false);
			}
		}
		break;

	case animationState::zoom:
		for (int k = 0; k < playerRockets_.size(); k++) {
			int dirY = 1;
			if (k < playerRockets_.size() * 0.5f) dirY = -1;
			playerRockets_[k]->setPosUIElement(b2Vec2(playerRockets_[k]->getPosUIElement().x + CONST(int, "ROCKET_SHAKE_X"), playerRockets_[k]->getPosUIElement().y + CONST(int, "ROCKET_SHAKE_Y") * dirY));
		}
		if (currentFrame >= frameZoomStateEnds_) {
			//Aqui pedir� a la super clase que ya se ponga el nuevo mapa y esas cosas
			SDL_Game::instance()->getAudioMngr()->resumeMusic();
			GameStateMachine* gsMachine = SDL_Game::instance()->getStateMachine();
			if (gsMachine->getMatchInfo()->getCurrentRoundNumber() < gsMachine->getMatchInfo()->getNumberOfRounds())
				gsMachine->transitionToState(States::play);
			else
				gsMachine->transitionToState(States::endGame);
		}

		//Altamente provisional hasta que sepamos hacer mates
		spaceStationScaleFactor_ += CONST(double, "SCALE_GROWTH");
		spaceStationViewer_->setScale(spaceStationScaleFactor_);

		//Numeros m�gicos yeiiii
		double new_x = (spaceStationViewer_->getPosUIElement().x - 3.0);
		double new_y = (spaceStationViewer_->getPosUIElement().y - 1.25);

		spaceStationViewer_->setScale(spaceStationScaleFactor_);
		spaceStationViewer_->setPosUIElement(b2Vec2(new_x, new_y));
		//NO SABEMOS HACER MATES.
		break;
	}
}

void MidGameState::handleInput()
{
	GameState::handleInput();
	InputHandler* ih = SDL_Game::instance()->getInputHandler();

	if (currentState != zoom)
	{
		//Has esperado para ver hasta donde se movia el cohete
		if (currentState == waitingButton && (ih->keyDownEvent() || ih->isButonDownEvent())) {
			buttonPush = true;
			continueText->setActive(false);
		}
		//Eres un prisa y solo quieres matar
		else if (currentState != waitingButton && (ih->keyDownEvent() || ih->isButonDownEvent())) {
			currentState = zoom;
			frameZoomStateEnds_ = currentFrame + CONST(int, "ZOOM_DURATION");
		}
	}
}

void MidGameState::onLoaded() {
	SDL_Game::instance()->getStateMachine()->deleteState(States::play);

	SDL_Game::instance()->getAudioMngr()->playMusic(Resources::TutorialMusic, -1);

	//Reseteamos el estado a como estaba en origen
	currentState = waiting;
	currentFrame = 0;
	spaceStationScaleFactor_ = 1;
	framesUntilNextShake_ = 0;
	spaceStationViewer_->setScale(spaceStationScaleFactor_);
	buttonPush = false;

	int initPos = (CONST(int, "WINDOW_HEIGHT") * 0.5f) - (-distanceBetweenRockets_ * 0.5f + (distanceBetweenRockets_ * 0.5f * numPlayers_));

	SDL_Rect rocketRect;
	rocketRect.x = rocketRect.y = 0;
	rocketRect.w = 160;
	rocketRect.h = 76;

	spaceStationViewer_->setPosUIElement(b2Vec2(CONST(int, "WINDOW_WIDTH") * 0.5f - CONST(int, "SPACE_STATION_WIDTH") * 0.5f, CONST(int, "WINDOW_HEIGHT") * 0.5f - CONST(int, "SPACE_STATION_HEIGHT") * 0.5f));

	vector<MatchInfo::PlayerInfo*>* playersInfo = SDL_Game::instance()->getStateMachine()->getMatchInfo()->getPlayersInfo();
	vector<int> playerPoints;
	for (int i = 0; i < playersInfo->size(); i++) {
		if (i == roundWinner_) {
			playerPoints.push_back((*playersInfo)[i]->totalPoints - 1);
		}
		else
			playerPoints.push_back((*playersInfo)[i]->totalPoints);
	}

	for (int i = 0; i < markers.size(); i++) {
		markers[i]->setActive(true);
	}

	for (int k = 0; k < playerRockets_.size(); k++) {
		playerRockets_[k]->setPosUIElement(b2Vec2(CONST(int, "START_ROCKET_POSITION") + distanceGainedByPoint_ * playerPoints[k], (initPos - 76) + (distanceBetweenRockets_ * k) + rocketRect.h * 0.5f));
	}
}