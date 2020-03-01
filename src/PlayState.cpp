#include "PlayState.h"
#include "Transform.h"
#include "Viewer.h"

PlayState::PlayState() :
	entityManager_(nullptr),
	physicsWorld_(nullptr) {
}

PlayState::~PlayState() {
	delete entityManager_;
	delete physicsWorld_;
}

void PlayState::init() {
	//aqui se crean todas las entidades necesarias
	//se podr�a JSONizar para evitar compilar
	entityManager_ = new EntityManager();
	physicsWorld_ = new b2World(b2Vec2(0, 0));

	b2BodyDef wallBodyDef;
	b2BodyDef tinkyBodyDef;
	tinkyBodyDef.type = b2_dynamicBody;
	tinkyBodyDef.position.Set(200, 200);
	wallBodyDef.position.Set(0, -500);

	b2Body* wallBody = physicsWorld_->CreateBody(&wallBodyDef);
	b2Body* tinkyBody = physicsWorld_->CreateBody(&tinkyBodyDef);

	b2PolygonShape boxShape;
	b2PolygonShape tinkyShape;
	boxShape.SetAsBox(1000,5);
	tinkyShape.SetAsBox(10, 10);
	
	wallBody->CreateFixture(&boxShape,0);
	tinkyBody->CreateFixture(&tinkyShape, 0);
	
	Entity* tinky = entityManager_->addEntity();
	Entity* ground = entityManager_->addEntity();
	Transform* tr = tinky->addComponent<Transform>(tinkyBody);
	Transform* trG = ground->addComponent<Transform>(wallBody);
	tinky->addComponent<Viewer>();
	ground->addComponent<Viewer>();
	//tr->getBody()->ApplyForce(b2Vec2 (0, -200), b2Vec2(0, 0), true);
	tr->getBody()->ApplyLinearImpulse(b2Vec2(0, -100), b2Vec2(0, 0),true);
}

void PlayState::update() {
	entityManager_->update();
	physicsWorld_->Step(1.0f / 60.0f, 6, 2);
	//tambi�n deber�a actualizar la l�gica de modo de juego
	//spawners de monedas, carga de objetivos...
}
void PlayState::render() {
	SDL_RenderClear(SDL_Game::instance()->getRenderer());

	entityManager_->render();

	SDL_RenderPresent(SDL_Game::instance()->getRenderer());
}
void PlayState::handleInput() {
	entityManager_->handleInput();
}