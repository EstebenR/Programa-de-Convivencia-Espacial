#pragma once
#include "ObjectPool.h"
#include "Entity.h"
#include "ObjectFactory.h"
#include "Bullet.h"

class BulletPool
{
private:
	ObjectPool<Entity, 10> bulletsPool_; //PROVISIONAL
public:
	BulletPool();
	~BulletPool() {}
	
	void init(EntityManager* eMan, b2World* physicsWorld);
	void addBullet(b2Vec2 pos, b2Vec2 size, b2Vec2 vel, int texture, int damage, int player);
};
