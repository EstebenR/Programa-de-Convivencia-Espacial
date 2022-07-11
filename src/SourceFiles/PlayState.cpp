#include "PlayState.h"
#include "Transform.h"
#include "AttachesToObjects.h"
#include "Texture.h"
#include "Viewer.h"
#include "Health.h"
#include "HealthViewer.h"
#include "InputBinder.h"
#include "CollisionHandler.h"
#include "TileMap.h"
#include "Tileson.h"
#include "Weapon.h"
#include "Hands.h"
#include "PlayerController.h"
#include "ObjectFactory.h"
#include "ImpulseViewer.h"
#include "PlayerData.h"
#include "ParticleEmitter.h"
#include "GameStateMachine.h"
#include "ThrownByPlayer.h"
#include "Component.h"
#include "Countdown.h"
#include "ColliderViewer.h"

PlayState::PlayState(GameMode* gMode, string tmap) :GameState(),
gameMode_(gMode), tilemapName_(tmap) {}


PlayState::~PlayState() {
	delete gameMode_;			gameMode_ = nullptr;
	delete tilemap_;			tilemap_ = nullptr;
	delete entityManager_;		entityManager_ = nullptr;
	delete collisionHandler_;	collisionHandler_ = nullptr;
	delete physicsWorld_;		physicsWorld_ = nullptr;
}

void PlayState::init() {
	//aqui se crean todas las entidades necesarias
	//se podría JSONizar para evitar compilar
	entityManager_ = new EntityManager();
	physicsWorld_ = new b2World(b2Vec2(0, 0));

	confettiPool_.init(entityManager_, physicsWorld_);
	staplerPool_.init(entityManager_, physicsWorld_, &bulletPool_);
	bodyPool_.init(entityManager_, physicsWorld_, collDeadBodies, deadBodies);
	/*bulletPool_.init(entityManager_, physicsWorld_);
	bananaPool_.init(entityManager_, physicsWorld_, &bulletPool_);*/

	secondsPerFrame_ = CONST(double, "SECONDS_PER_FRAME");

	tilemap_ = new TileMap(CONST(double, "WINDOW_WIDTH"), CONST(double, "WINDOW_HEIGHT"),
		resourcesPath + "tilemaps/" + tilemapName_ + ".json",
		entityManager_, physicsWorld_, &bulletPool_, &confettiPool_, &staplerPool_, gameMode_);
	tilemap_->init();
	gameMode_->setTileMap(tilemap_);

	collisionHandler_ = new CollisionHandler(gameMode_, tilemap_);
	physicsWorld_->SetContactListener(collisionHandler_);

	//Particle test
	/*ParticleEmitter* test = players_[0]->addComponent<ParticleEmitter>(Vector2D(.5,.5),Resources::Stapler,3.5,1000, 25, 0,2,15);
	test->PlayStop();*/

	//FONDO
	fondo_ = SDL_Game::instance()->getTexturesMngr()->getTexture(resourceMap_[tilemapName_]);

	//MÚSICA
	SDL_Game::instance()->getAudioMngr()->playMusic(resourceMap_[tilemapName_], -1);

	//Version estática de la factoria
	tilemap_->executeMapFactory();
	tilemap_->createWeapons();

	gameMode_->init(this);

	playerInfo = SDL_Game::instance()->getStateMachine()->getMatchInfo()->getPlayersInfo();

	for (Weapon* w : *(entityManager_->getWeaponVector())) {
		w->getEntity()->addComponent<ThrownByPlayer>(gameMode_);
	}

	Entity* countdown = entityManager_->addEntity();
	Countdown* count = countdown->addComponent<Countdown>(gameMode_);
	count->assignBoiler(tilemap_->getFireballGen());
}

void PlayState::update() {
	GameState::update();
	//double d = SDL_Game::instance()->getConstants()->getConstant<double>((std::string)"SECONDS_PER_FRAME");
	//el que vuelva tocar el step de physicsworld muere
	physicsWorld_->Step(secondsPerFrame_, 6, 2);
	collisionHandler_->SolveInteractions();
	createDeadBodies();
	//también debería actualizar la lógica de modo de juego
	//spawners de monedas, carga de objetivos...
	gameMode_->update();
}

void PlayState::render() {
	fondo_->render(0, 0);
	GameState::render();
	gameMode_->render();
}

void PlayState::handleInput()
{
	GameState::handleInput();
	for (MatchInfo::PlayerInfo* pInfo : *playerInfo)
	{
		if (pInfo->inputBinder->pressPause()) {
			SDL_Game::instance()->getAudioMngr()->pauseMusic();
			SDL_Game::instance()->getStateMachine()->setPauseOwner(pInfo->playerId);
			//SDL_Game::instance()->getStateMachine()->transitionToState(States::pause);
		}
	}
}

void PlayState::createDeadBodies() {
	auto bodies = collisionHandler_->getBodyData();
	for (int i = 0; i < bodies.size(); i++) {
		bodyPool_.addBody(bodies[i].pos, bodies[i].angle, bodies[i].linearVelocity, bodies[i].angularVelocity);
	}
	/*if (deadBodies.size() < maxCorpses_) {
		for (int i = 0; i < bodies.size(); i++) {
			ObjectFactory::makeDeadBody(entityManager_, physicsWorld_, collDeadBodies, deadBodies,
				bodies[i].pos, bodies[i].angle, bodies[i].linearVelocity, bodies[i].angularVelocity);
		}
	}*/
	collisionHandler_->clearBodyData();
}