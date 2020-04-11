#pragma once
#include "ObjectPool.h"
#include "Entity.h"

class EntityManager;
class b2World;
class b2Vec2;

class CoinsPool {
private:
	ObjectPool<Entity, 140> coinsPool_;
	int coinMaxSpeed_ = 0;
public:
	CoinsPool();
	~CoinsPool() {};
	void init(EntityManager* eMan, b2World* physicsWorld);

	void addCoin(b2Vec2 pos, int player = -1, int val = 1);
};
