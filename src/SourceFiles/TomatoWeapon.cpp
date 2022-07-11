#include "TomatoWeapon.h"
#include "Collider.h"
#include "Entity.h"
#include "Collision.h"
#include "Health.h"
#include "Wallet.h"
#include "PlayerData.h"
#include "AnimatedViewer.h"
#include "InputHandler.h"
#include "ParticleEmitter.h"
#include "Hands.h"
#include "CollisionHandler.h"
#include "ThrownByPlayer.h"
#include "Constants.h"

void TomatoWeapon::init() {
	ActionableWeapon::init();
	colTomato_ = GETCMP1_(Collider);
	tomatoViewer_ = entity_->getComponent<AnimatedViewer>(ComponentType::Viewer);
	tomatoViewer_->stopAnimation();
	particleEmitterTomato_ = GETCMP1_(ParticleEmitter);

	framesCharge_ = CONST(float, "TOMATO_TIME_CHARGE") * FRAMES_PER_SECOND;
	framesExplosion_ = CONST(float, "TOMATO_TIME_EXPLOSION") * FRAMES_PER_SECOND;
	nFramesCharge_ = CONST(int, "TOMATO_N_FRAMES_ACTIVATED");
	nFramesExplosion_ = CONST(int, "TOMATO_N_FRAMES_EXPLOSION");
	damageOnExplosionImpact_ = CONST(int, "TOMATO_DAMAGE");
	explosionSize_ = CONST(int, "TOMATO_EXPLOSION_SIZE");
	explosionForce_ = CONST(int, "TOMATO_EXPLOSION_FORCE");

	timePerFrameCharge_ = framesCharge_ / (nFramesCharge_ - 1);
	timePerFrameExplosion_ = framesExplosion_ / (nFramesExplosion_ - 1);
}

void TomatoWeapon::update() {
	Weapon::update();
	if (currentHand_) mainCollider_->setTransform(currentHand_->getPointerPos(), 0.0);

	if (activated_) {
		currentFrame_++;
		if (currentFrame_ == framesCharge_) {
			colTomato_->createCircularFixture(explosionSize_, 0, 0, 0, Collider::CollisionLayer::NormalObject, true);
			exploded_ = true;
			activated_ = false;
			if (picked_) UnPickObject();
			tomatoViewer_->startAnimation(0, nFramesCharge_ - 1, tomatoViewer_->getTexture()->getNumFramesX());
			tomatoViewer_->setAnimSpeed(timePerFrameExplosion_);
			colTomato_->setLinearVelocity({ 0,0 });
			SDL_Game::instance()->getAudioMngr()->playChannel(Resources::TomatoExplosionSound, 0);
			currentFrame_ = 0;
		}
	}
	else if (exploded_) {
		currentFrame_++;
		if (currentFrame_ == framesExplosion_) {
			setActive(false);
		}
	}
}

void TomatoWeapon::onCollisionEnter(Collision* c) {
	Weapon::onCollisionEnter(c);

	if (exploded_) {
		Entity* other = c->entity;

		Health* healthPlayer = GETCMP2(other, Health);
		Wallet* walletPlayer = GETCMP2(other, Wallet);
		PlayerData* playerData = GETCMP2(other, PlayerData);
		Collider* collPlayer = GETCMP2(other, Collider);

		ThrownByPlayer* objThrown = GETCMP1_(ThrownByPlayer);

		if (healthPlayer && collPlayer) {
			if (!healthPlayer->subtractLife(damageOnExplosionImpact_)) {
				// add points to owner
				if (objThrown->getOwnerId() != playerData->getPlayerNumber())
					objThrown->addPointsToOwner();

				healthPlayer->playerDead(c->collisionHandler);
			}
		}
		else if (walletPlayer && collPlayer) {
			c->collisionHandler->addCoinDrop(std::make_tuple(walletPlayer, playerData, 3));
		}

		b2Vec2 dir = (collPlayer->getPos() - colTomato_->getPos());
		dir.Normalize();
		if (dir.x == 0 && dir.y == 0) { //This happens when the tomato explodes in the hand of the player.
			int dirX = rand() % 2;
			int dirY = rand() % 2;

			if (dirX == 0) dirX = -1;
			if (dirY == 0) dirY = -1;

			dir.x = (float)(rand() % 100) / 100 * dirX;
			dir.y = 1 - dir.x * dirY;
		}
		collPlayer->applyForce({ dir.x * explosionForce_, dir.y * explosionForce_ }, { 0,0 });
	}
}

void TomatoWeapon::action() {
	if (!activated_) {
		activated_ = true;
		currentHand_->startAnimation(0, 0, nFramesCharge_);
		currentHand_->setAnimSpeed(timePerFrameCharge_);
		particleEmitterTomato_->setPositionCollider(colTomato_);
		particleEmitterTomato_->setDirection({ 0, -1 });
		particleEmitterTomato_->PlayStop();
	}
}

void TomatoWeapon::PickObjectBy(Hands* playerHands)
{
	if (playerHands->getWeaponID() == NoWeapon) {
		currentHand_ = playerHands;
		picked_ = true;
		currentHand_->setWeapon(weaponType_, this);
		vw_->setDrawable(false);
		b2Filter pickUpCollider = mainCollider_->getFixture(0)->GetFilterData();
		pickUpCollider.categoryBits = 0;
		pickUpCollider.maskBits = 0;
		mainCollider_->getFixture(0)->SetFilterData(pickUpCollider);

		SDL_Game::instance()->getAudioMngr()->playChannel(Resources::PickSound, 0);
	}
}

void TomatoWeapon::UnPickObject() {
	b2Filter pickUpCollider = mainCollider_->getFixture(0)->GetFilterData();
	pickUpCollider.categoryBits = Collider::CollisionLayer::NormalObject;
	pickUpCollider.maskBits = Collider::CollisionLayer::NormalObject | Collider::CollisionLayer::NormalAttachableObject | Collider::CollisionLayer::Player | Collider::CollisionLayer::Wall | Collider::CollisionLayer::NonGrababbleWall;
	mainCollider_->getFixture(0)->SetFilterData(pickUpCollider);

	ActionableWeapon::UnPickObject();

	if (activated_ && !exploded_) {
		tomatoViewer_->startAnimation(0, currentFrame_ / framesCharge_, nFramesCharge_);
	}
}

void TomatoWeapon::setActive(bool a, b2Vec2 pos) {
	entity_->setActive(a);
	tomatoViewer_->setDrawable(a);
	colTomato_->getBody()->SetEnabled(a);
	if (exploded_) {
		currentFrame_ = 0;
		tomatoViewer_->setFrame(0);
		colTomato_->destroyFixture(2);
	}

	if (a) colTomato_->getBody()->SetTransform(pos, 0);
	else {
		tomatoViewer_->stopAnimation();
		exploded_ = false;
		activated_ = false;
	}
}