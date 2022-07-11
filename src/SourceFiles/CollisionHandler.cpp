#include "CollisionHandler.h"
#include "RouterLogic.h"
#include "StocksGameMode.h"
#include "Collision.h"
#include <tuple>
#include "Fireball.h"
#include "FireBallGenerator.h"
//This method calculates the damage recieved by the impact of an object (or another player) with the player


//Handles start of collisions
void CollisionHandler::BeginContact(b2Contact* contact)
{
	b2Fixture* fixA = contact->GetFixtureA();
	b2Fixture* fixB = contact->GetFixtureB();

	if (fixA == nullptr || fixB == nullptr) return;

	Entity* aEnt = static_cast<Entity*>((void*)fixA->GetBody()->GetUserData().pointer);
	Entity* bEnt = static_cast<Entity*>((void*)fixB->GetBody()->GetUserData().pointer);

	if (aEnt != nullptr && bEnt != nullptr)
	{
		Collision col = Collision(fixB, contact, bEnt, this, fixA);
		aEnt->onCollisionEnter(&col);
		col = Collision(fixA, contact, aEnt, this, fixB);
		bEnt->onCollisionEnter(&col);
	}
}

//Handles end of collisions
void CollisionHandler::EndContact(b2Contact* contact) {
	b2Fixture* fixA = contact->GetFixtureA();
	b2Fixture* fixB = contact->GetFixtureB();

	if (fixA == nullptr || fixB == nullptr) return;

	Entity* aEnt = static_cast<Entity*>((void*)fixA->GetBody()->GetUserData().pointer);
	Entity* bEnt = static_cast<Entity*>((void*)fixB->GetBody()->GetUserData().pointer);

	if (aEnt != nullptr && bEnt != nullptr)
	{
		Collision col = Collision(fixB, contact, bEnt, this, fixA);
		aEnt->onCollisionExit(&col);
		col = Collision(fixA, contact, aEnt, this, fixB);
		bEnt->onCollisionExit(&col);
	}
}

//If you want to disable a collision after it's detected
void CollisionHandler::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
}

//Gather info about impulses
void CollisionHandler::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {}

void CollisionHandler::SolveInteractions() {
	for (auto data : vecWeld) { //Recorre el vector resolviendo todos los joint y lo limpia al final.
		data.player->attachToObject(data.bodyToBeAttached, data.collPoint, data.collNormal);
	}
	vecWeld.clear();
	for (auto weapon : vecWeapon) { //Recorre el vector soltando los weapon y lo limpia al final.
		weapon->letFallObject();
	}
	vecWeapon.clear();
	for (auto attach : vecAttach) { //Recorre el vector soltando los agarres y lo limpia al final.
		attach->deAttachFromObject();
	}
	vecAttach.clear();
	for (auto move : vecMove) { //Recorre el vector resolviendo todos los move y lo limpia al final.
		move.body->SetTransform(move.pos, 0);
		move.body->SetLinearVelocity(b2Vec2_zero);
		move.body->SetAngularVelocity(0);
	}
	vecMove.clear();
	for (auto c : vecCoin) {
		c->setActive(false);
	}
	vecCoin.clear();
	for (auto f : fireballsToClear) {
		f->setActive(false, b2Vec2(3, 3));
	}
	fireballsToClear.clear();
	for (auto w : vecCoinsToDrop) {
		std::get<0>(w)->dropCoins(std::get<2>(w), std::get<1>(w)->getPlayerNumber());
	}
	vecCoinsToDrop.clear();
}