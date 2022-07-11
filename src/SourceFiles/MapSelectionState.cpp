#include "MapSelectionState.h"
#include "InputHandler.h"
#include "SDL_Game.h"
#include "GameStateMachine.h"
#include "Texture.h"
#include "UIViewer.h"
#include "Constants.h"
#include "checkML.h"
#include "InputBinder.h"
#include "MatchInfo.h"

void MapSelectionState::init() {
	if (entityManager_ != nullptr) entityManager_->~EntityManager();
	entityManager_ = new EntityManager();
	ownerPlayerBinder_ = SDL_Game::instance()->getStateMachine()->getMatchInfo()->getPlayersInfo()->at(ownerPlayerID_)->inputBinder;

	loadConst();
	createImages();
	createTexts();
	createIcons();
	//cout << "init map select\n";

	Entity* temp = entityManager_->addEntity();
	mapSelCursor_ = temp->addComponent<UIViewer>(Resources::CursorUiSelectMap, coordsImages_[0] + mapCursorOffset_, mapScale_ * 3, 0);

	backgroundTex_ = SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::RocketRoom);

	//MUSICA
	SDL_Game::instance()->getAudioMngr()->playMusic(Resources::AudioId::MainMenuMusic, -1);

	//Textos
	choseMapText_ = entityManager_->addEntity();
	UIViewer* uiV = choseMapText_->addComponent<UIViewer>(Resources::ChoseMap, b2Vec2(0, 0), 1);
	uiV->setPosUIElement(b2Vec2((CONST(int, "WINDOW_WIDTH") - uiV->getW()) * 0.5f, CONST(int, "CHOSE_MAP_TEXT_Y_OFFSET")));

	choseModeText_ = entityManager_->addEntity();
	uiV = choseModeText_->addComponent<UIViewer>(Resources::ChoseMode, b2Vec2(0, 0), 1);
	uiV->setPosUIElement(b2Vec2((CONST(int, "WINDOW_WIDTH") - uiV->getW()) * 0.5f, CONST(int, "CHOSE_MAP_TEXT_Y_OFFSET")));
	choseModeText_->setActive(false);

	pressEnterText_ = entityManager_->addEntity();
	uiV = pressEnterText_->addComponent<UIViewer>(Resources::PressEnter, b2Vec2(0, 0), 1);
	uiV->setPosUIElement(b2Vec2((CONST(int, "WINDOW_WIDTH") - uiV->getW()) * 0.5f, (CONST(int, "WINDOW_HEIGHT") - CONST(int, "CHOSE_MAP_TEXT_Y_OFFSET") * 2)));
	pressEnterText_->setActive(false);
}

void MapSelectionState::handleInput()
{
	GameState::handleInput();
	//mover arriba y abajo
	if (!holdingY_)
	{
		if (ownerPlayerBinder_->menuMove(Dir::Down)) {
			updatePointer(-1, y);
			holdingY_ = true;
		}
		else if (ownerPlayerBinder_->menuMove(Dir::Up)) {
			updatePointer(1, y);
			holdingY_ = true;
		}
	}
	else if (!ownerPlayerBinder_->menuMove(Dir::Up) && !ownerPlayerBinder_->menuMove(Dir::Down)) {
		holdingY_ = false;
	}

	if (!holdingX_)
	{
		if (ownerPlayerBinder_->menuMove(Dir::Right)) {
			updatePointer(1, x);
			holdingX_ = true;
		}
		else if (ownerPlayerBinder_->menuMove(Dir::Left)) {
			updatePointer(-1, x);
			holdingX_ = true;
		}
	}
	else if (!ownerPlayerBinder_->menuMove(Dir::Right) && !ownerPlayerBinder_->menuMove(Dir::Left)) {
		holdingX_ = false;
	}


	if (ownerPlayerBinder_->menuForward()) {
		if (menuPointer_ == 0) { //Seleccionando mapa
			menuPointer_++;
			updateMenu();
		}
		else { //Elegimos el modo de juego
			addRound((GamemodeID)(pointers_[gamemodeScreen]), maps_[pointers_[x] + pointers_[y] * 2]);
			if (roundsVector_->size() == maxNumberOfRounds_) {
				SDL_Game::instance()->getStateMachine()->getMatchInfo()->setRounds(roundsVector_);
				SDL_Game::instance()->getStateMachine()->transitionToState(States::play);
				iconsGamemodes_[pointers_[gamemodeScreen]]->setActive(false);
			}
			else { //como onLoaded pero sin resetear la m�sica
				menuPointer_ = 0;
				pointers_[x] = 0;
				pointers_[y] = 0;
				updateMenu();
			}
		}
	}
	//ir para atr�s
	else if (ownerPlayerBinder_->menuBack())
	{
		if (menuPointer_ == 1) {
			menuPointer_--;
			updateMenu();
		}
		else if (menuPointer_ == 0) {
			removeRound(maps_[pointers_[x] + pointers_[y] * 2]);
		}
	}

	if (ownerPlayerBinder_->pressPause() && menuPointer_ == 0 && roundsVector_->size() != 0) {
		SDL_Game::instance()->getStateMachine()->getMatchInfo()->setRounds(roundsVector_);
		SDL_Game::instance()->getStateMachine()->transitionToState(States::play, roundsVector_->front().first, roundsVector_->front().second);
		iconsGamemodes_[pointers_[gamemodeScreen]]->setActive(false);
	}
}

void MapSelectionState::addRound(GamemodeID gMode, string map) {
	roundsVector_->push_back(std::make_pair(gMode, map));

	int indexMap;

	if (map == maps_[0]) {
		indexMap = 0;
	}
	else if (map == maps_[1]) {
		indexMap = 1;
	}
	else if (map == maps_[2]) {
		indexMap = 2;
	}
	else if (map == maps_[3]) {
		indexMap = 3;
	}

	b2Vec2 pos = coordsImages_[indexMap] + b2Vec2{ iconsMaps_[indexMap].size() * mapIconMargin_ + mapIconsOffset_.x, mapIconsOffset_.y };

	iconsMaps_[indexMap].push_back(std::pair(entityManager_->addEntity(), (GamemodeID)gMode));

	iconsMaps_[indexMap].back().first->addComponent<UIViewer>(texturesIcons_[gMode], pos, mapIconsScale_, 0);
}

void MapSelectionState::removeRound(string map)
{
	int indexMap = 0;

	if (map == maps_[0]) {
		indexMap = 0;
	}
	else if (map == maps_[1]) {
		indexMap = 1;
	}
	else if (map == maps_[2]) {
		indexMap = 2;
	}
	else if (map == maps_[3]) {
		indexMap = 3;
	}

	if (!iconsMaps_[indexMap].empty()) {
		GamemodeID gamemode = iconsMaps_[indexMap].back().second;

		bool found = false;

		auto a = --roundsVector_->end();

		int i = iconsMaps_[indexMap].size() - 1;

		while (!found && a >= roundsVector_->begin()) {
			auto b = *a;

			if (*a == std::pair{ gamemode, map }) {
				roundsVector_->erase(a);
				(iconsMaps_[indexMap].begin() + i)->first->setActive(false);
				iconsMaps_[indexMap].erase(iconsMaps_[indexMap].begin() + i);
				found = true;
			}
			else {
				if (a->second == map) i--;
				a--;
			}
		}
	}
}

void MapSelectionState::onLoaded() { //poner el men� al principio
	SDL_Game::instance()->getAudioMngr()->playMusic(Resources::AudioId::MainMenuMusic, -1);
	SDL_Game::instance()->getStateMachine()->deleteState(States::play);
	menuPointer_ = 0;
	pointers_[x] = 0;
	pointers_[y] = 0;
	pointers_[gamemodeScreen] = 0;
	roundsVector_->clear();
	for (auto& item : iconsMaps_) {
		for (auto& pairs : item) {
			delete pairs.first;
		}
		item.clear();
	}
	updateMenu();

	SDL_Game::instance()->getStateMachine()->deleteState(States::endGame);
}

void MapSelectionState::updatePointer(int n, int coor) {
	int size = 0;

	if (menuPointer_ == 0) {
		size = 2;	 //Mapas por columna / fila

		pointers_[x] += (n * (coor + 1) + size);	//para evitar numeros negativos sumamos size
		pointers_[x] %= size;
		pointers_[y] += (n * (coor)+size);
		pointers_[y] %= size;

		mapSelCursor_->setPosUIElement(coordsImages_[pointers_[x] + pointers_[y] * 2] + mapCursorOffset_);
	}
	else {
		size = GamemodeID::NUMBER_OF_GAMEMODES - 1; //sin tutorial

		iconsGamemodes_[pointers_[gamemodeScreen]]->setActive(false);

		pointers_[gamemodeScreen] += size - n; // -n porque en la segunda pantalla usamos un sistema de coordenadas distinto
		pointers_[gamemodeScreen] %= size;

		iconsGamemodes_[pointers_[gamemodeScreen]]->setActive(true);
	}
}

void MapSelectionState::loadConst()
{
	gmOffsetText_ = { CONST(float, "GM_OFFSET_TEXT_X"),  CONST(float, "GM_OFFSET_TEXT_Y") };
	gmOffsetIcons_ = { CONST(float, "GM_OFFSET_ICONS_X"),  CONST(float, "GM_OFFSET_ICONS_Y") };
	gmMarginText_ = { CONST(float, "GM_MARGIN_TEXT_X"),  CONST(float, "GM_MARGIN_TEXT_Y") };

	gmIconsSize_ = CONST(int, "GM_ICONS_SIZE");

	gmTextSize_ = CONST(int, "GM_TEXT_SIZE");
	gmTextScale_ = CONST(float, "GM_TEXT_SCALE");

	mapCursorOffset_ = { CONST(float, "MAP_CURSOR_OFFSET_X"),CONST(float, "MAP_CURSOR_OFFSET_Y") };
	mapScale_ = CONST(float, "MAP_SCALE");
	mapMargin_ = CONST(int, "MAP_MARGIN");

	mapIconMargin_ = CONST(int, "MAP_ICON_MARGIN");
	mapIconsOffset_ = { CONST(float, "MAP_ICON_OFFSET_X"),CONST(float, "MAP_ICON_OFFSET_Y") };

	mapIconsScale_ = CONST(int, "MAP_ICON_SCALE");
	sizeImg = { CONST(float, "MAP_SIZE_IMAGE_X"), CONST(float, "MAP_SIZE_IMAGE_Y") };
	sizeScreen = { CONST(float, "WINDOW_WIDTH"), CONST(float, "WINDOW_HEIGHT") };
}

void MapSelectionState::updateMenu() {
	for (auto& e : textsGamemodes_) {
		e->setActive(menuPointer_ == 1);
	}

	iconsGamemodes_[pointers_[gamemodeScreen]]->setActive(menuPointer_ == 1);

	for (auto& e : imagesMaps_) {
		e->setActive(menuPointer_ != 1);
	}

	choseMapText_->setActive(menuPointer_ != 1);
	pressEnterText_->setActive(menuPointer_ != 1);
	choseModeText_->setActive(menuPointer_ == 1);

	for (auto& i : iconsMaps_) {
		for (auto& j : i) {
			j.first->setActive(menuPointer_ != 1);
		}
	}

	mapSelCursor_->setDrawable(menuPointer_ != 1);
	mapSelCursor_->setPosUIElement(coordsImages_[pointers_[x] + pointers_[y] * 2] + mapCursorOffset_);
};

void MapSelectionState::createImages() { //preparar los textos
	coordsImages_.push_back(b2Vec2((sizeScreen.x - (sizeImg.x * 2 + mapMargin_)) * 0.5f,
		(sizeScreen.y - (sizeImg.y * 2 + mapMargin_)) * 0.5f));

	coordsImages_.push_back(b2Vec2((sizeScreen.x - (sizeImg.x * 2 + mapMargin_)) * 0.5f + mapMargin_ + sizeImg.x,
		(sizeScreen.y - (sizeImg.y * 2 + mapMargin_)) * 0.5f));

	coordsImages_.push_back(b2Vec2((sizeScreen.x - (sizeImg.x * 2 + mapMargin_)) * 0.5f,
		(sizeScreen.y - (sizeImg.y * 2 + mapMargin_)) * 0.5f + mapMargin_ + sizeImg.y));

	coordsImages_.push_back(b2Vec2((sizeScreen.x - (sizeImg.x * 2 + mapMargin_)) * 0.5f + mapMargin_ + sizeImg.x,
		(sizeScreen.y - (sizeImg.y * 2 + mapMargin_)) * 0.5f + mapMargin_ + sizeImg.y));

	for (int i = 0; i < 4; i++) {
		imagesMaps_.push_back(entityManager_->addEntity());
		imagesMaps_.back()->addComponent<UIViewer>(texturesMaps_[i], coordsImages_[i], mapScale_, 0);
	}
}

void MapSelectionState::createTexts()
{
	int i = 0;

	int winWidth = CONST(float, "WINDOW_WIDTH");
	for (auto e : texturesTexts_) {
		textsGamemodes_.push_back(entityManager_->addEntity());
		UIViewer* uiV = textsGamemodes_.back()->addComponent<UIViewer>(texturesTexts_[i], b2Vec2(0, 0), gmTextScale_, 0);
		uiV->setPosUIElement(b2Vec2((winWidth - uiV->getW()) * 0.5f, (float)(i * gmMarginText_.y + gmOffsetText_.y)));
		textsGamemodes_.back()->setActive(false);
		i++;
	}
}

void MapSelectionState::createIcons() {
	int i = 0;

	int winWidth = CONST(float, "WINDOW_WIDTH");
	int margin = CONST(float, "GM_ICON_OFFSET");

	for (auto e : texturesIcons_) {
		iconsGamemodes_.push_back(entityManager_->addEntity());
		UIViewer* uiV = iconsGamemodes_.back()->addComponent<UIViewer>(texturesIcons_[i], b2Vec2(0, 0), 3, 0);
		UIViewer* uiVText = GETCMP2(textsGamemodes_[i], UIViewer);
		uiV->setPosUIElement(b2Vec2((winWidth - uiVText->getW()) * 0.5f - margin, (float)(i * gmMarginText_.y + gmOffsetIcons_.y)));
		iconsGamemodes_.back()->setActive(false);
		i++;
	}

	for (auto e : iconsMaps_) {
		e.erase(e.begin(), e.end());
	}
}

void MapSelectionState::render() {
	backgroundTex_->render(0, 0);
	GameState::render();
}