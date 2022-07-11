#include "HealthViewer.h"
#include "Entity.h"
#include "PlayerData.h"

HealthViewer::HealthViewer(int texActiveId, int texDisabledId, int skin) : Component(ComponentType::HealthViewer)
{
	activeTextureId_ = texActiveId;
	disableTextureId_ = texDisabledId;
	playerSkin_ = skin;
}

void HealthViewer::init()
{
	//Diria que no hace falta poner nada mas...
	playerData_ = GETCMP1_(PlayerData);
	he = GETCMP1_(Health);
	full = SDL_Game::instance()->getTexturesMngr()->getTexture(activeTextureId_);
	empty = SDL_Game::instance()->getTexturesMngr()->getTexture(disableTextureId_);
	int playerNumber = playerData_->getPlayerNumber();
	setPos(playerNumber);
	flip_ = (playerNumber % 2 == 0) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

	lifeWidth_ = CONST(int, "LIFE_WIDTH");
	lifeHeight_ = CONST(int, "LIFE_HEIGTH");
	lifeDrawOffset_ = CONST(double, "LIFE_DRAW_OFFSET");
}
void HealthViewer::update()
{
	//Todo lo hace el draw, este no sirve ninguna funci�n
}
void HealthViewer::draw() const
{
	int player = playerData_->getPlayerNumber();
	SDL_Rect dest = { (int)pos_.x, (int)pos_.y, lifeWidth_, lifeHeight_ };
	for (int i = 0; i < he->getHealthMax(); i++) {
		// +- offset dependiendo de la posicion del jugador
		dest.x = pos_.x + ((player % 2 == 0) ? 1 : -1) * i * (lifeWidth_ + lifeDrawOffset_);

		if ((i + 1) <= he->getHealth())	//Si tiene esas X vidas las muertra como llenas
			full->render(dest, 0, 0, playerSkin_, flip_);
		else		//Si no las tiene, se dibujar�n como vacias
			empty->render(dest, 0, 0, 0, flip_);
	}
}

void HealthViewer::handleInput() {
}

void HealthViewer::setPos(int player) {
	pos_.x = (player % 2 == 0) ? (CONST(int, "LIFE_MARGIN_H")) : (CONST(int, "WINDOW_WIDTH") - CONST(int, "LIFE_MARGIN_H") - CONST(int, "LIFE_WIDTH"));
	pos_.y = (player < 2) ? (CONST(double, "LIFE_MARGIN_V")) : (CONST(int, "WINDOW_HEIGHT") - CONST(double, "LIFE_MARGIN_V") - CONST(int, "LIFE_HEIGTH"));
}