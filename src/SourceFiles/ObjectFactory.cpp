#include "ObjectFactory.h"

#include "AnimatedViewer.h"
#include "BoilerButtonLogic.h"
#include "Bullet.h"
#include "BulletPool.h"
#include "CarnivorousPlant.h"
#include "Coin.h"
#include "ColliderViewer.h"
#include "EntityManager.h"
#include "FireBallGenerator.h"
#include "Fireball.h"
#include "GameMode.h"
#include "Hands.h"
#include "Health.h"
#include "HealthViewer.h"
#include "InputHandler.h"
#include "Pad.h"
#include "ParticleEmitter.h"
#include "SpawnTree.h"
#include "Texture.h"
#include "ThrownByPlayer.h"
#include "Transform.h"
#include "Treadmill.h"
#include "Viewer.h"
#include "Weapon.h"
#include "WeaponPool.h"
#include "WiFiBullet.h"
#include "TriggerButton.h"
#include "WeaponSpawner.h"
#include "ConfettiPool.h"
#include "RouterLogic.h"

Entity* ObjectFactory::makeTable(EntityManager* entityManager, b2World* physicsWorld, b2Vec2 pos, b2Vec2 size)
{
	Entity* e = entityManager->addEntity();
	Collider* aux = e->addComponent<Collider>(physicsWorld, b2_dynamicBody, pos.x, pos.y, size.x, size.y, CONST(double, "TABLE_DENSITY"),
		CONST(double, "TABLE_FRICTION"), CONST(double, "TABLE_RESTITUTION"),
		CONST(double, "TABLE_LINEAR_DRAG"), CONST(double, "TABLE_ANGULAR_DRAG"), Collider::CollisionLayer::NormalAttachableObject, false);
	e->addComponent<Transform>(SDL_Rect{ 0,0,CONST(int, "TABLE_W_SPRITE"),CONST(int, "TABLE_H_SPRITE") }, aux);
	e->addComponent<Viewer>(Resources::Table);

	//e->addComponent<ColliderViewer>();

	return e;
}

Entity* ObjectFactory::makeLamp(EntityManager* entityManager, b2World* physicsWorld, b2Vec2 pos, b2Vec2 size)
{
	Entity* e = entityManager->addEntity();
	Collider* aux = e->addComponent<Collider>(physicsWorld, b2_dynamicBody, pos.x, pos.y, size.x, size.y, CONST(double, "LAMP_DENSITY"),
		CONST(double, "LAMP_FRICTION"), CONST(double, "LAMP_RESTITUTION"),
		CONST(double, "LAMP_LINEAR_DRAG"), CONST(double, "LAMP_ANGULAR_DRAG"), Collider::CollisionLayer::NormalAttachableObject, false);
	e->addComponent<Transform>(SDL_Rect{ 0,0,CONST(int, "LAMP_W_SPRITE"),CONST(int, "LAMP_H_SPRITE") }, aux);
	e->addComponent<Viewer>(Resources::Lamp);

	//e->addComponent<ColliderViewer>();

	return e;
}

Entity* ObjectFactory::makeSofa(EntityManager* entityManager, b2World* physicsWorld, b2Vec2 pos, b2Vec2 size)
{
	Entity* e = entityManager->addEntity();
	Collider* aux = e->addComponent<Collider>(physicsWorld, b2_dynamicBody, pos.x, pos.y, size.x, size.y, CONST(double, "SOFA_DENSITY"),
		CONST(double, "SOFA_FRICTION"), CONST(double, "SOFA_RESTITUTION"),
		CONST(double, "SOFA_LINEAR_DRAG"), CONST(double, "SOFA_ANGULAR_DRAG"), Collider::CollisionLayer::NormalAttachableObject, false);
	e->addComponent<Transform>(SDL_Rect{ 0,0,CONST(int, "SOFA_W_SPRITE"),CONST(int, "SOFA_H_SPRITE") }, aux);
	e->addComponent<Viewer>(Resources::Sofa);

	//e->addComponent<ColliderViewer>();

	return e;
}

Weapon* ObjectFactory::makeController(EntityManager* entityManager, b2World* physicsWorld, b2Vec2 pos, b2Vec2 size) {
	Entity* e = entityManager->addEntity();
	Collider* aux = e->addComponent<Collider>(physicsWorld, b2_dynamicBody, pos.x, pos.y, size.x, size.y, CONST(double, "CONTROLLER_DENSITY"), CONST(double, "CONTROLLER_FRICTION"),
		CONST(double, "CONTROLLER_RESTITUTION"), CONST(double, "CONTROLLER_LINEAR_DRAG"), CONST(double, "CONTROLLER_ANGULAR_DRAG"), Collider::CollisionLayer::NormalObject, false);
	e->addComponent<Transform>(SDL_Rect{ 0,0, CONST(int, "CONTROLLER_W_SPRITE"),CONST(int, "CONTROLLER_H_SPRITE") }, aux);
	e->addComponent<Viewer>(Resources::Remote);
	Weapon* controller = e->addComponent<Weapon>(WeaponID::Mando, CONST(int, "CONTROLLER_IMPACT_DAMAGE"));
	//e->addComponent<ColliderViewer>();

	return controller;
}

Entity* ObjectFactory::makeWall(EntityManager* entityManager, b2World* physicsWorld, b2Vec2 pos, b2Vec2 size)
{
	Entity* e = entityManager->addEntity();								 // x, y,width, height, density,friction, restitution, linearDrag, angularDrag,	Layer, sensor
	Collider* aux = e->addComponent<Collider>(physicsWorld, b2_staticBody, pos.x, pos.y, size.x, size.y, 10, 1, 0.2, 0, 0, Collider::CollisionLayer::Wall, false);
	e->addComponent<Transform>(SDL_Rect{ 0,0,	CONST(int, "WALLS_BASE_W_SPRITE") + (int)(size.x * (int)CONST(double, "PIXELS_PER_METER") * CONST(float, "WALLS_SCALE_W_SPRITE")),
												CONST(int, "WALLS_BASE_H_SPRITE") + (int)(size.y * (int)CONST(double, "PIXELS_PER_METER") * CONST(float, "WALLS_SCALE_W_SPRITE")) }, aux);
	e->addComponent<Viewer>(Resources::Negro);
	//e->addComponent<ColliderViewer>();

	return e;
}

Entity* ObjectFactory::makePipe(EntityManager* entityManager, b2World* physicsWorld, b2Vec2 pos, b2Vec2 size, float rotation) {
	Entity* e = entityManager->addEntity();								 // x, y,width, height, density,friction, restitution, linearDrag, angularDrag,	Layer, sensor
	Collider* aux = e->addComponent<Collider>(physicsWorld, b2_staticBody, pos.x, pos.y, size.x, size.y, 10, 1, 0.2, 0, 0, Collider::CollisionLayer::Wall, false);
	e->addComponent<Transform>(SDL_Rect{ 0,0,(int)size.x * CONST(int, "PIXELS_PER_METER") ,(int)size.y * CONST(int, "PIXELS_PER_METER") }, aux);
	e->addComponent<Transform>(SDL_Rect{ 0,0,	CONST(int, "PIPES_BASE_W_SPRITE") + (int)(size.x * (int)CONST(double, "PIXELS_PER_METER") * CONST(float, "PIPES_SCALE_W_SPRITE")),
												CONST(int, "PIPES_BASE_H_SPRITE") + (int)(size.y * (int)CONST(double, "PIXELS_PER_METER") * CONST(float, "PIPES_SCALE_H_SPRITE")) }, aux);
	//Por si se debe de invertir la imagen en tuberias horizontales
	if (size.x > size.y) {
		e->addComponent<Viewer>(Resources::PipeHor);
		if (pos.y > CONST(int, "WINDOW_HEIGHT") / CONST(double, "PIXELS_PER_METER") * 0.5f) {
			aux->getBody()->SetTransform(pos, PI);
		}
	}
	//Por si se debe de invertir la imagen en tuberias verticales
	else {
		e->addComponent<Viewer>(Resources::PipeVer);
		if (pos.x > CONST(int, "WINDOW_WIDTH") / CONST(double, "PIXELS_PER_METER") * 0.5f) {
			aux->getBody()->SetTransform(pos, -PI);
		}
	}
	//Si la tuberia tiene una rotacion especial se la aplicamos
	if (rotation != 0) { aux->getBody()->SetTransform(pos, rotation * (-PI) / 180); }

	//e->addComponent<ColliderViewer>();

	return e;
}
Entity* ObjectFactory::makeTrasparentWall(EntityManager* entityManager, b2World* physicsWorld, b2Vec2 pos, b2Vec2 size, float rotation) {
	Entity* e = entityManager->addEntity();								 // x, y,width, height, density,friction, restitution, linearDrag, angularDrag,	Layer, sensor
	Collider* aux = e->addComponent<Collider>(physicsWorld, b2_staticBody, pos.x, pos.y, size.x, size.y, 10, 1, 0.2, 0, 0, Collider::CollisionLayer::Wall, false);
	//e->addComponent<Transform>(SDL_Rect{ 0,0,(int)size.x, (int)size.y }, aux);
	//e->addComponent<ColliderViewer>();

	return e;
}

Entity* ObjectFactory::makeSpaceJunk(EntityManager* entityManager, b2World* physicsWorld, b2Vec2 pos, b2Vec2 size)
{
	Entity* e = entityManager->addEntity();
	Collider* aux = e->addComponent<Collider>(physicsWorld, b2_dynamicBody, pos.x, pos.y, CONST(float, "SPACE_JUNK_W_PHYSICS"), CONST(float, "SPACE_JUNK_H_PHYSICS"), 1, 0.1, 0.2, 0, 0, Collider::CollisionLayer::NormalObject, false);
	e->addComponent<Transform>(SDL_Rect{ 0, 0, CONST(int, "SPACE_JUNK_H_SPRITE"), CONST(int, "SPACE_JUNK_W_SPRITE") }, aux);
	e->addComponent<Viewer>(Resources::Stone);

	aux->applyLinearImpulse(b2Vec2(rand() % 4, rand() % 4), b2Vec2(0, 0));
	//e->addComponent<ColliderViewer>();

	return e;
}

Entity* ObjectFactory::makePad(EntityManager* entityManager, b2World* physicsWorld, b2Vec2 pos, b2Vec2 size)
{
	Entity* e = entityManager->addEntity();
	Collider* aux = e->addComponent<Collider>(physicsWorld, b2_staticBody, pos.x, pos.y, size.x, size.y, 0, 0, 1, 0, 0, Collider::CollisionLayer::NonGrababbleWall, false);
	e->addComponent<Transform>(SDL_Rect{ 0,0, CONST(int, "PAD_BASE_W_SPRITE") + (int)(size.x * (int)CONST(double, "PIXELS_PER_METER") * CONST(float, "PAD_SCALE_W_SPRITE")),
												CONST(int, "PAD_BASE_H_SPRITE") + (int)(size.y * (int)CONST(double, "PIXELS_PER_METER") * CONST(float, "PAD_SCALE_W_SPRITE")) }, aux);
	e->addComponent<AnimatedViewer>(Resources::PadSpriteSheet, CONST(int, "PAD_ANIMATION_SPEED"))->stopAnimation();
	e->addComponent<Pad>();
	//e->addComponent<ColliderViewer>();

	return e;
}

Entity* ObjectFactory::makeCoin(Entity* e, EntityManager* entityManager, b2World* physicsWorld, b2Vec2 pos, int value)
{
	entityManager->addExistingEntity(e);
	Collider* aux = e->addComponent<Collider>(physicsWorld, b2_dynamicBody, pos.x, pos.y, CONST(double, "COIN_BASE_SIZE"), CONST(double, "COIN_DENSITY"), CONST(double, "COIN_FRICTION"), CONST(double, "COIN_ANGULAR_DRAG"), 1, 1, Collider::CollisionLayer::Trigger, true);
	e->addComponent<Transform>(SDL_Rect{ 0,0,CONST(int, "COIN_RADIUS_SPRITE"),CONST(int, "COIN_RADIUS_SPRITE") }, aux);
	e->addComponent<Viewer>(Resources::Coin);
	e->addComponent<Coin>();
	//e->addComponent<ColliderViewer>();

	return e;
}

Entity* ObjectFactory::makeFireball(Entity* e, EntityManager* entityManager, b2World* physicsWorld, b2Vec2 pos)
{
	entityManager->addExistingEntity(e);
	Collider* auxCol = e->addComponent<Collider>(physicsWorld, b2_dynamicBody, pos.x, pos.y, CONST(double, "FIREBALL_RADIUS_PHYSICS"), CONST(double, "FIREBALL_DENSITY"), CONST(double, "FIREBALL_FRICTION"), CONST(double, "FIREBALL_RESTITUTION"), CONST(double, "FIREBALL_LINEAR_DRAG"), CONST(double, "FIREBALL_ANGULAR_DRAG"), Collider::CollisionLayer::NormalObject, false);
	e->addComponent<Transform>(SDL_Rect{ 0,0, CONST(int, "FIREBALL_RADIUS_SPRITE"),CONST(int, "FIREBALL_RADIUS_SPRITE") }, auxCol);
	e->addComponent<AnimatedViewer>(Resources::Fireball, CONST(int, "FIREBALL_TIME_PER_FRAME"));
	e->addComponent<Fireball>();
	e->addComponent<Weapon>(WeaponID::Piedra, 999);//Si, el fuego es una piedra muy caliente. Mucho m�s que el magma, esta caldera echa bolas de PLASMA, el cuarto estado de la materia
	//e->addComponent<ColliderViewer>();

	return e;
}

Entity* ObjectFactory::makeRoomba(EntityManager* entityManager, b2World* physicsWorld, b2Vec2 pos)
{
	Entity* e = entityManager->addEntity();
	Collider* collRoomba = e->addComponent<Collider>(physicsWorld, b2_dynamicBody, pos.x, pos.y, CONST(double, "ROOMBA_RADIUS_PHYSICS"), CONST(double, "ROOMBA_DENSITY"), CONST(double, "ROOMBA_FRICTION"),
		CONST(double, "ROOMBA_RESTITUTION"), CONST(double, "ROOMBA_LINEAR_DRAG"), CONST(double, "ROOMBA_ANGULAR_DRAG"), Collider::CollisionLayer::UnInteractableObject, false);

	e->addComponent<Transform>(SDL_Rect{ 0,0, CONST(int, "ROOMBA_RADIUS_SPRITE"), CONST(int, "ROOMBA_RADIUS_SPRITE") }, collRoomba);
	e->addComponent<AnimatedViewer>(Resources::RoombaSpriteSheet, CONST(int, "ROOMBA_TIMEPERFRAME"));

	double velocityX = rand() % CONST(int, "ROOMBA_VELOCITY");
	double velocityY = CONST(int, "ROOMBA_VELOCITY") - velocityX;

	if (rand() % 2 == 1)velocityX *= -1;
	if (rand() % 2 == 1) velocityY *= -1;

	collRoomba->applyLinearImpulse(b2Vec2(velocityX, velocityY), b2Vec2(0, 0));
	//e->addComponent<ColliderViewer>();

	return e;
}

Entity* ObjectFactory::makeBoiler(EntityManager* entityManager, b2World* physicsWorld, b2Vec2 pos)
{
	Entity* e = entityManager->addEntity();
	Collider* collBoiler = e->addComponent<Collider>(physicsWorld, b2_staticBody, pos.x, pos.y, CONST(double, "BOILER_W_PHYSICS"), CONST(double, "BOILER_H_PHYSICS"), CONST(double, "BOILER_DENSITY"), CONST(double, "BOILER_FRICTION"),
		CONST(double, "BOILER_RESTITUTION"), CONST(double, "BOILER_LINEAR_DRAG"), CONST(double, "BOILER_ANGULAR_DRAG"), Collider::CollisionLayer::NonGrababbleWall, false);
	int nFrames = SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::Smoke)->getWidth() / SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::Smoke)->getHeight();
	e->addComponent<ParticleEmitter>(Vector2D(0, -1), Resources::Smoke, CONST(int, "FBGEN_PARTICLE_SPEED"), nFrames, CONST(int, "FBGEN_PARTICLE_GEN_ODDS"), CONST(int, "FBGEN_PARTICLE_LIFETIME"), CONST(int, "FBGEN_PARTICLE_SIZE"), 0, CONST(int, "FBGEN_PARTICLE_SPEED_VARIATION"), 180);
	e->addComponent<Transform>(SDL_Rect{ 0,0, CONST(int, "BOILER_W_SPRITE"), CONST(int, "BOILER_H_SPRITE") }, collBoiler);
	e->addComponent<AnimatedViewer>(Resources::Boiler, CONST(int, "BOILER_ANIMATION_SPEED"));
	e->addComponent<FireBallGenerator>(physicsWorld);
	//e->addComponent<ColliderViewer>();

	return e;
}

Entity* ObjectFactory::makeBoilerButton(EntityManager* entityManager, b2World* physicsWorld, b2Vec2 pos, bool inc_dec) {
	Entity* e = entityManager->addEntity();
	Collider* collBoilerButton = e->addComponent<Collider>(physicsWorld, b2_staticBody, pos.x, pos.y, CONST(double, "BOILER_BUTTON_RADIUS_PHYSICS"), 0, 0,
		0, 0, 0, Collider::CollisionLayer::NormalObject, true);
	e->addComponent<Transform>(SDL_Rect{ 0,0, CONST(int, "BOILER_BUTTON_RADIUS_SPRITE"),CONST(int, "BOILER_BUTTON_RADIUS_SPRITE") }, collBoilerButton);
	e->addComponent<AnimatedViewer>((inc_dec) ? Resources::IncreasingFreqButton : Resources::DecreasingFreqButton, 0)->stopAnimation();
	e->addComponent<BoilerButtonLogic>(inc_dec);
	//e->addComponent<ColliderViewer>();

	return e;
}

Entity* ObjectFactory::makeTreadmill(EntityManager* entityManager, b2World* physicsWorld, b2Vec2 pos)
{
	float offset = CONST(double, "TREADMILL_MILL_W_PHYSICS") + CONST(double, "TREADMILL_HANDLE_W_PHYSICS");

	Entity* h = entityManager->addEntity();
	Collider* collHandle = h->addComponent<Collider>(physicsWorld, b2_kinematicBody, pos.x, pos.y, CONST(double, "TREADMILL_HANDLE_W_PHYSICS"), CONST(double, "TREADMILL_HANDLE_H_PHYSICS"), CONST(double, "TREADMILL_DENSITY"), CONST(double, "TREADMILL_FRICTION"),
		CONST(double, "TREADMILL_RESTITUTION"), CONST(double, "TREADMILL_LINEAR_DRAG"), CONST(double, "TREADMILL_ANGULAR_DRAG"), Collider::CollisionLayer::NormalAttachableObject, false);
	h->addComponent<Transform>(SDL_Rect{ 0,0,CONST(int, "TREADMILL_HANDLE_W_SPRITE"),CONST(int, "TREADMILL_HANDLE_H_SPRITE") }, collHandle);
	h->addComponent<Viewer>(Resources::TreadmillPanel);


	Entity* m = entityManager->addEntity();
	Collider* collMill = m->addComponent<Collider>(physicsWorld, b2_dynamicBody, pos.x + offset, pos.y, CONST(double, "TREADMILL_MILL_W_PHYSICS"), CONST(double, "TREADMILL_MILL_H_PHYSICS"), CONST(double, "TREADMILL_DENSITY"), CONST(double, "TREADMILL_FRICTION"),
		CONST(double, "TREADMILL_RESTITUTION"), CONST(double, "TREADMILL_LINEAR_DRAG"), CONST(double, "TREADMILL_ANGULAR_DRAG"), Collider::CollisionLayer::NormalObject, true);
	m->addComponent<Transform>(SDL_Rect{ 0,0,CONST(int, "TREADMILL_MILL_W_SPRITE"),CONST(int, "TREADMILL_MILL_H_SPRITE") }, collMill);
	m->addComponent<AnimatedViewer>(Resources::TreadmillSpriteSheet, CONST(int, "TREADMILL_ANIMATION_SPEED"));

	m->addComponent<Treadmill>(collHandle);

	b2Vec2 collPoint = b2Vec2(pos.x + offset, pos.y);
	b2WeldJointDef jointDef; //Definición del nuevo joint.
	jointDef.bodyA = collMill->getBody(); //Body del jugador.
	jointDef.bodyB = collHandle->getBody(); //Body del objeto al que se tiene que atar.
	jointDef.collideConnected = false; //Flag que decide si estos 2 objetos van a ejercer físicas el uno contra el otro.
	jointDef.localAnchorA = jointDef.bodyA->GetLocalPoint(collPoint); //Punto donde se ata el cuerpo A al cuerpo B
	jointDef.localAnchorB = jointDef.bodyB->GetLocalPoint(collPoint); //Punto donde se ata el cuerpo B al cuerpo A
	jointDef.referenceAngle = jointDef.bodyB->GetAngle() - jointDef.bodyA->GetAngle(); //Ángulo conjunto del cuerpo
	b2World* world = collMill->getWorld(); //Obtenemos el mundo físico para crear el joint
	world->CreateJoint(&jointDef); //Crea el joint con la definición que hemos definido previamente
	//h->addComponent<ColliderViewer>();
	//m->addComponent<ColliderViewer>();

	return m;
}

Entity* ObjectFactory::makeCarnivorousPlant(EntityManager* entityManager, b2World* physicsWorld, b2Vec2 pos, b2Vec2 size)
{
	Entity* plant = entityManager->addEntity();
	double diff = CONST(double, "CARNIVOROUSPLANT_H_SPRITE") / CONST(double, "CARNIVOROUSPLANT_W_SPRITE");
	Collider* collPlant = plant->addComponent<Collider>(physicsWorld, b2_staticBody, pos.x, pos.y, size.x, size.y * diff, 0, 0, 0, 0, 0, Collider::CollisionLayer::Trigger, true);
	plant->addComponent<Transform>(SDL_Rect{ 0,0,CONST(int,"CARNIVOROUSPLANT_W_SPRITE") * (int)size.x, CONST(int,"CARNIVOROUSPLANT_H_SPRITE") * (int)size.y }, collPlant);
	plant->addComponent<CarnivorousPlantViewer>(Resources::CarnivorousPlant, CONST(int, "CARNIVOROUSPLANT_MIN_SPEED"));
	plant->addComponent<CarnivorousPlant>();
	//plant->addComponent<ColliderViewer>();

	return plant;
}

Entity* ObjectFactory::makeBullet(Entity* e, EntityManager* entityManager, b2World* physicsWorld, GameMode* gameMode)
{
	entityManager->addExistingEntity(e);

	Collider* aux = e->addComponent<Collider>(physicsWorld, b2_dynamicBody, 0, 0, CONST(double, "BULLET_W_PHYSICS"), CONST(double, "BULLET_H_PHYSICS"), //estos 0.7 son la escala, es provisional hasta que se pueda cambiar mas adelante en el setActive
		CONST(double, "BULLET_DENSITY"), CONST(double, "BULLET_FRICTION"), CONST(double, "BULLET_RESTITUTION"),
		CONST(double, "BULLET_LINEAR_DRAG"), CONST(double, "BULLET_ANGULAR_DRAG"), Collider::CollisionLayer::NormalObject, true);
	e->addComponent<Transform>(SDL_Rect{ 0,0, CONST(int, "BULLET_W_SPRITE"),CONST(int, "BULLET_H_SPRITE") }, aux);
	e->addComponent<Viewer>(Resources::Negro);

	e->addComponent<Bullet>();
	//e->addComponent<ColliderViewer>();
	e->addComponent<ThrownByPlayer>(gameMode);

	return e;
}

Entity* ObjectFactory::makeTomatoTree(EntityManager* entityManager, b2World* physicsWorld, b2Vec2 pos, WeaponPool* pool) {
	Texture* tomatoTex = SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::TreeTomato);
	Entity* e = entityManager->addEntity();
	Collider* col = e->addComponent<Collider>(physicsWorld, b2_kinematicBody, pos.x, pos.y, CONST(double, "SPAWN_TREE_W_PHYSICS"),
		CONST(double, "SPAWN_TREE_H_PHYSICS"), CONST(double, "SPAWN_TREE_DENSITY"), CONST(double, "SPAWN_TREE_FRICTION"),
		CONST(double, "SPAWN_TREE_RESTITUTION"), CONST(double, "SPAWN_TREE_LINEAR_DRAG"), CONST(double, "SPAWN_TREE_ANGULAR_DRAG"),
		Collider::CollisionLayer::UnInteractableObject, false);
	e->addComponent<Transform>(SDL_Rect{ 0,0,CONST(int, "SPAWN_TREE_W_SPRITE") , CONST(int, "SPAWN_TREE_H_SPRITE") }, col);
	e->addComponent<Viewer>(Resources::TomatoTree);
	//SDL_Rect clip;
	//clip.h = tomatoTex->getHeight();
	//clip.w = tomatoTex->getWidth() / 17;
	//clip.x = 0; clip.y = 0;
	e->addComponent<SpawnTree>(tomatoTex, CONST(double, "TOMATO_RADIUS_PHYSICS"),
		CONST(double, "TOMATO_RADIUS_PHYSICS"), pool, entityManager, physicsWorld);

	return e;
}

Entity* ObjectFactory::makeBananaTree(EntityManager* entityManager, b2World* physicsWorld, b2Vec2 pos, WeaponPool* pool) {
	Texture* bananaTex = SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::Banana);
	Entity* e = entityManager->addEntity();
	Collider* col = e->addComponent<Collider>(physicsWorld, b2_kinematicBody, pos.x, pos.y, CONST(double, "SPAWN_TREE_W_PHYSICS"),
		CONST(double, "SPAWN_TREE_H_PHYSICS"), CONST(double, "SPAWN_TREE_DENSITY"), CONST(double, "SPAWN_TREE_FRICTION"),
		CONST(double, "SPAWN_TREE_RESTITUTION"), CONST(double, "SPAWN_TREE_LINEAR_DRAG"), CONST(double, "SPAWN_TREE_ANGULAR_DRAG"),
		Collider::CollisionLayer::UnInteractableObject, false);
	e->addComponent<Transform>(SDL_Rect{ 0,0,CONST(int, "SPAWN_TREE_W_SPRITE") , CONST(int, "SPAWN_TREE_H_SPRITE") }, col);
	e->addComponent<Viewer>(Resources::BananaTree);
	e->addComponent<SpawnTree>(bananaTex, CONST(double, "BANANA_W_PHYSICS"),
		CONST(double, "BANANA_H_PHYSICS"), pool, entityManager, physicsWorld);

	return e;
}

Entity* ObjectFactory::makeRouter(EntityManager* entityManager, b2World* physicsWorld, b2Vec2 pos, WiFightGameMode* gMode, WiFiWavePool* wavePool) {
	Entity* router = entityManager->addEntity();
	//				x,								y,						width, height, density,	friction, restitution, linearDrag, angularDrag,	 Laye,  sensor,  canBeAttached
	Collider* collRouter = router->addComponent<Collider>(physicsWorld, b2_dynamicBody, pos.x, pos.y, 1, 0.7, 1, 0, 1, 0, 0, Collider::CollisionLayer::UnInteractableObject, false);
	collRouter->createCircularFixture(5, 1, 0, 0, Collider::CollisionLayer::Trigger, true);
	router->addComponent<Transform>(SDL_Rect{ 0,0,CONST(int, "ROUTER_W_SPRITE"), CONST(int, "ROUTER_H_SPRITE") }, collRouter);
	router->addComponent<Viewer>(Resources::Router);
	router->addComponent<RouterLogic>(gMode, wavePool);
	//router->addComponent<ColliderViewer>();

	collRouter->applyLinearImpulse(b2Vec2(100, 100), b2Vec2(0, 0));

	return router;
}

Entity* ObjectFactory::makeWifiWave(Entity* e, EntityManager* entityManager, b2World* physicsWorld, Collider* colRouter)
{
	entityManager->addExistingEntity(e);

	Collider* aux = e->addComponent<Collider>(physicsWorld, b2_dynamicBody, 0, 0, 0.7, 0.7, //estos 0.7 son la escala, es provisional hasta que se pueda cambiar mas adelante en el setActive
		CONST(double, "BULLET_DENSITY"), CONST(double, "BULLET_FRICTION"), CONST(double, "BULLET_RESTITUTION"),
		CONST(double, "BULLET_LINEAR_DRAG"), CONST(double, "BULLET_ANGULAR_DRAG"), Collider::CollisionLayer::Trigger, true);


	Texture* t = SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::WiFiWave);

	e->addComponent<Transform>(SDL_Rect{ 0,0,CONST(int, "WIFI_W_SPRITE"), CONST(int, "WIFI_H_SPRITE") }, aux);
	e->addComponent<AnimatedViewer>(Resources::WiFiWave, 100);
	e->addComponent<WiFiBullet>(colRouter);
	//e->addComponent<ColliderViewer>();

	return e;
}

Entity* ObjectFactory::makeTriggerButton(EntityManager* entityManager, b2World* physicsWorld, b2Vec2 pos, b2Vec2 size, string state) {
	Entity* trButton = entityManager->addEntity();
	Collider* trigger = trButton->addComponent<Collider>(physicsWorld, b2_staticBody, pos.x, pos.y, size.x, size.y, 0, 0, 0, 0, 0, Collider::CollisionLayer::Trigger, true);
	//trButton->addComponent<Transform>(SDL_Rect{ 0,0,(int)size.x, (int)size.y }, trigger);
	trButton->addComponent<Transform>(SDL_Rect{ 0,0,	CONST(int, "WALLS_BASE_W_SPRITE") + (int)(size.x * (int)CONST(double, "PIXELS_PER_METER") * CONST(float, "WALLS_SCALE_W_SPRITE")),
												CONST(int, "WALLS_BASE_H_SPRITE") + (int)(size.y * (int)CONST(double, "PIXELS_PER_METER") * CONST(float, "WALLS_SCALE_W_SPRITE")) }, trigger);

	Resources::TextureId texId;
	if (state == "Play") texId = Resources::TriggerButtonPlay;
	else if (state == "Options") texId = Resources::TriggerButtonOptions;
	else if (state == "Credits") texId = Resources::TriggerButtonCredits;
	else if (state == "Exit") texId = Resources::TriggerButtonExit;
	trButton->addComponent<AnimatedViewer>(texId, CONST(int, "TRIGGER_BUTTON_ANIM_SPEED"));

	trButton->addComponent<TriggerButton>(state);
	//trButton->addComponent<ColliderViewer>();

	return trButton;
}

Entity* ObjectFactory::makeWeaponSpawner(EntityManager* entityManager, b2World* physicsWorld, b2Vec2 position, ConfettiPool* confettiPool, StaplerPool* staplerPool, BulletPool* bulletPool)
{
	Entity* e = entityManager->addEntity();
	e->addComponent<WeaponSpawner>(position, entityManager, physicsWorld, confettiPool, staplerPool, bulletPool);

	return e;
}

void ObjectFactory::makeDeadBody(Entity* e, EntityManager* entityManager, b2World* physicsWorld, vector<Collider*>& collDeadBodies, vector<Entity*>& deadBodies, b2Vec2 pos, float angle, b2Vec2 linearVelocity, float angularVelocity) {
	entityManager->addExistingEntity(e);
	deadBodies.push_back(e);
	collDeadBodies.push_back(deadBodies.back()->addComponent<Collider>(physicsWorld, b2_dynamicBody, pos.x, pos.y, CONST(double, "PLAYER_W_PHYSICS"), CONST(double, "PLAYER_H_PHYSICS"),
		CONST(double, "PLAYER_DENSITY"), CONST(double, "PLAYER_FRICTION"), CONST(double, "PLAYER_RESTITUTION"), CONST(double, "PLAYER_LINEAR_DRAG"), CONST(double, "PLAYER_ANGULAR_DRAG"), Collider::CollisionLayer::NormalAttachableObject, false));
	deadBodies.back()->addComponent<Transform>(SDL_Rect{ 0,0,CONST(int, "CORPSE_W_SPRITE"),CONST(int, "CORPSE_H_SPRITE") }, collDeadBodies.back());
	deadBodies.back()->addComponent<Viewer>(Resources::DeadBody);
	//deadBodies.back()->addComponent<ColliderViewer>();
}