#pragma once
#include <box2d.h>
#include <iostream>
#include "Health.h"
//#include "AttachesToObjects.h"
#include "Weapon.h"
#include "Entity.h"
#include "Hands.h"
#include "Collider.h"
#include "PlayerData.h"
#include "GameMode.h"
#include "TileMap.h"
#include "Pad.h"
#include "Wallet.h"
#include "Coin.h"
#include "checkML.h"

class Fireball;
class RouterLogic;
class CollisionHandler :
	public b2ContactListener
{
public:
	struct weldData;
	struct moveData;
	struct bodyData;
	
private:
	vector<weldData> vecWeld; //Vector donde almacenamos los welds que realizaremos al final del step.

	vector<moveData> vecMove; //Vector donde almacenamos los moves que realizaremos al final del step.
	vector<bodyData> vecBody; //Vector donde almacenamos los cuerpos muertos que crearemos al final del step.
	vector<Weapon*> vecWeapon; //Vector donde almacenamos las weapons que soltaremos al final del step.
	vector<AttachesToObjects*> vecAttach; //Vector donde almacenamos los agarres que soltaremos al final del step.
	vector<tuple<Wallet*, PlayerData*, int>> vecCoinsToDrop; //Vector donde almacenamos los impactos entre objetos y wallets.
	vector<Fireball*> fireballsToClear; //Bolas de fuego que se eliminaran al final del step
	vector<Coin*> vecCoin; //Vector donde almacenamos los impactos entre jugador y las monedas a recoger
	GameMode* gMode_;
	TileMap* tilemap_;


public:
	struct weldData { //Struct donde guardamos los datos necesarios para hacer un weld.
		AttachesToObjects* player = nullptr;
		b2Body* bodyToBeAttached = nullptr;
		b2Vec2 collPoint;
		b2Vec2 collNormal;
		weldData() {};
		weldData(AttachesToObjects* attach, b2Body* body, b2Vec2 colPt, b2Vec2 colNor) :
			player(attach), bodyToBeAttached(body), collPoint(colPt), collNormal(colNor) {}
	};
	struct moveData { //Struct donde guardamos los datos necesarios para respawnear a los jugadores
		b2Body* body;
		b2Vec2 pos;
	};
	struct bodyData {  //Struct donde guardamos los datos necesarios para hacer un cuerpo muerto.
		b2Vec2 pos;
		float angle;
		b2Vec2 linearVelocity;
		float angularVelocity;
	};
	CollisionHandler(GameMode* g, TileMap* tm) : gMode_(g), tilemap_(tm) {};
	~CollisionHandler() {};

	void BeginContact(b2Contact* contact);

	void EndContact(b2Contact* contact);

	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);

	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);

	void SolveInteractions();

	vector<bodyData> getBodyData() { return vecBody; }

	void clearBodyData() { vecBody.clear(); }

	void createWeld(weldData w) { vecWeld.push_back(w); }

	void breakAttachment(AttachesToObjects* a) { vecAttach.push_back(a); };

	void dropWeapon(Weapon* w) { vecWeapon.push_back(w); }

	GameMode* getGamemode() { return gMode_; }

	b2Vec2 getPlayerRespawnPoint(int player) { return tilemap_->getPlayerSpawnPoint(player); }

	void addCorpse(bodyData corpse) { vecBody.push_back(corpse); }

	void addMove(moveData mov) { vecMove.push_back(mov); }

	void addCoinDrop(std::tuple<Wallet*, PlayerData*, int> coin) { vecCoinsToDrop.push_back(coin); }

	void addCoinPick(Coin* coin) { vecCoin.push_back(coin); }

	void removeFireball(Fireball* fball) { fireballsToClear.push_back(fball); }
};
