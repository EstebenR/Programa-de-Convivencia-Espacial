#include "RouterLogic.h"
#include "Entity.h"
#include "WiFightGameMode.h"
#include "Collision.h"

void RouterLogic::detectPlayer(Collider* playerDetected, int id)
{
	PlayerInfo newPlayer;
	newPlayer.posPlayer = playerDetected;
	newPlayer.id = id;
	playersInsideRange_.push_back(newPlayer);
}

void RouterLogic::loseContactPlayer(Collider* playerDetected, int id) {
	vector<PlayerInfo>::iterator it = playersInsideRange_.begin();
	while (it->id != id && it->posPlayer != playerDetected && it != playersInsideRange_.end()) {
		++it;
	}
	if (it != playersInsideRange_.end()) playersInsideRange_.erase(it);
}

void RouterLogic::init()
{
	posRouter_ = GETCMP1_(Collider);
	frameWhenUpdate_ = CONST(int, "ROUTER_FRAMES_UNTIL_SHOOT");
	wifiWaveSpd_ = CONST(double, "ROUTER_SHOOT_SPEED");
}

void RouterLogic::update()
{	//5 should be a constant and points should be based on distance from the router and number of players in the area. More players inside = less points for everyone else.
	int numJug = playersInsideRange_.size();
	for (int k = 0; k < numJug; k++) {
		// puntos a�adidos = 1 / (numJugadores * (distancia + 1) para no dividir entre 0)
		double points = CONST(int, "POINTS_WHEN_INSIDE_ROUTER") / (numJug * ((playersInsideRange_[k].posPlayer->getPos() - posRouter_->getPos()).Length() + 1));
		wifightGameMode_->addPoints(playersInsideRange_[k].id, min(points, CONST(double, "MAX_POINTS_PER_TICK")));

		if (actFrame_ >= frameWhenUpdate_) {
			Collider* c = playersInsideRange_[k].posPlayer;
			b2Vec2 dir = c->getPos() - posRouter_->getPos();

			b2Vec2 aux = dir;
			aux.Normalize();

			wifiPool_->addBullet(posRouter_->getPos(), { c->getW(0) * (float)points, c->getH(0) * (float)points },
				wifiWaveSpd_ * points * aux,
				Resources::WiFiWave, 0, playersInsideRange_[k].id);
			actFrame_ = 0;
		}
		else actFrame_++;
	}
}

void RouterLogic::onCollisionEnter(Collision* c)
{
	PlayerData* playerData = GETCMP2(c->entity, PlayerData);
	if (playerData != nullptr) {
		detectPlayer(GETCMP2(c->entity, Collider), playerData->getPlayerNumber());
	}
}

void RouterLogic::onCollisionExit(Collision* c)
{
	PlayerData* playerData = GETCMP2(c->entity, PlayerData);
	if (playerData != nullptr) {
		loseContactPlayer(GETCMP2(c->entity, Collider), playerData->getPlayerNumber());
	}
}