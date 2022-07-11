#include "Bullet.h"
#include "ThrownByPlayer.h"
#include <cmath>

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

void Bullet::init()
{
	winWidth_ = CONST(int, "WINDOW_WIDTH");
	winHeight_ = CONST(int, "WINDOW_HEIGHT");
	col_ = GETCMP1_(Collider);
	viewer_ = GETCMP1_(Viewer);
}

void Bullet::update()
{
	if (SDL_Game::instance()->getTime() > limitTime_ ||					//se elimina si se acaba su tiempo de vida
		(col_->getPos().x < 0 || col_->getPos().y > winWidth_ ||	//se elimina si se sale de la pantalla
			col_->getPos().y < 0 || col_->getPos().y > winHeight_)) {
		needToDelete = true;
	}

	if (needToDelete) {
		setActive(false);
		needToDelete = false;
	}
}

void Bullet::setActive(bool a, b2Vec2 pos, b2Vec2 size, b2Vec2 vel, int texture, int damage, int player)
{
	entity_->setActive(a);
	viewer_->setDrawable(a);
	col_->getBody()->SetEnabled(a);

	if (a) { //hacer movidas
		float attachAngle = atanf(vel.y / vel.x) - (M_PI_2);
		if (vel.x < 0)attachAngle += PI; //se invierte
		col_->getBody()->SetTransform(pos, attachAngle);
		col_->applyLinearImpulse(vel, { 0,0 });
		//SIZE------------
		viewer_->setTexture(texture);
		damage_ = damage;
		player_ = player;
		limitTime_ = SDL_Game::instance()->getTime() + CONST(double, "BULLET_DESTROY_TIME");
	}
	else {
		col_->setLinearVelocity(b2Vec2(0, 0));//resetea su velocidad
		col_->getBody()->SetTransform(b2Vec2(0, 0), 0); //resetea su rotacion y posicion
	}
}

void Bullet::onCollisionEnter(Collision* c)
{
	Entity* cEntity = c->entity;
	PlayerData* pData = GETCMP2(cEntity, PlayerData);

	if (pData != nullptr && pData->getPlayerNumber() != player_) { //solo colisiona con jugadores que no sea el que lo ha lanzado
		Health* pHealth_ = GETCMP2(cEntity, Health);
		if (pHealth_ != nullptr) {
			if (!pHealth_->subtractLife(damage_)) {
				pHealth_->playerDead(c->collisionHandler);
				needToDelete = true;
				ThrownByPlayer* objThrown = GETCMP1_(ThrownByPlayer);
				if (objThrown->getOwnerId() != pData->getPlayerNumber())
					objThrown->addPointsToOwner();
			}
			needToDelete = true;
		}
		else {
			Wallet* wallet = GETCMP2(cEntity, Wallet);
			if (wallet != nullptr) {
				c->collisionHandler->addCoinDrop(std::make_tuple(wallet, pData, CONST(int, "BULLET_COIN_DMG")));
				needToDelete = true;
			}
		}
	}
	else { //dentro de un else para que si ha colisionado con un player ni se moleste en entrar aqui
		Collider* col = GETCMP2(cEntity, Collider);
		if (col != nullptr) {
			if (col->getFixture(0)->GetFilterData().categoryBits == Collider::CollisionLayer::Wall ||
				col->getFixture(0)->GetFilterData().categoryBits == Collider::CollisionLayer::NonGrababbleWall)
				needToDelete = true;
		}
	}
}