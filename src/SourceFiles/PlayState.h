#pragma once
#include <list>
#include "GameState.h"
#include "GameMode.h"
#include "Entity.h"
#include "checkML.h"
#include "CollisionHandler.h"
#include "Resources.h"
#include "MatchInfo.h"
#include "BananaPool.h"
#include "BulletPool.h"
#include "ConfettiPool.h"
#include "StaplerPool.h"
#include "BodyPool.h"

using namespace std;

// esta clase es equivalente a un nivel del juego
//se espera que herede de gamestate en un futuro
//tambien deberia convertirse en un template de modo de juego y mapa
class PlayState : public GameState
{
protected:
	b2World* physicsWorld_;
	vector<b2Body*> physicalEntities_; //almacena los punteros a los colliders de b2
	//puede que no sea necesario si cogemos la referencia en cuanto los creamos con addPhysicalEntity
	b2Body* addPhysicalEntity(/*PAR�METROS PARA PODER CREAR UN BODY*/) { return nullptr; };  //a�ade una entidad fisica
	//HUD

	CollisionHandler* collisionHandler_;
	vector<Entity*> deadBodies;
	vector<Collider*> collDeadBodies;
	GameMode* gameMode_ = nullptr;
	double secondsPerFrame_ = 0.016;
	string tilemapName_;
	Texture* fondo_;
	std::map<string, Resources::TextureId> resourceMap_ = { //añadir aquí los mapas que se vayan haciendo
		{"LivingRoom",Resources::LivingRoom},
		{"BoilerRoom",Resources::BoilerRoom},
		{"GymRoom",Resources::GymRoom},
		{"TutorialRoom", Resources::TutorialRoom},
		{"GardenRoom",Resources::GardenRoom},
		{"MenuRoom",Resources::MenuRoom}
	};
	TileMap* tilemap_;
	vector<MatchInfo::PlayerInfo*>* playerInfo;
	BulletPool bulletPool_;
	ConfettiPool confettiPool_;
	StaplerPool staplerPool_;
	BodyPool bodyPool_;

	double playerHeight_ = CONST(double, "PLAYER_H_PHYSICS");
	double playerWidth_ = CONST(double, "PLAYER_W_PHYSICS");
	double playerDensity_ = CONST(double, "PLAYER_DENSITY");
	double playerFriction_ = CONST(double, "PLAYER_FRICTION");
	double playerRestitution_ = CONST(double, "PLAYER_RESTITUTION");
	double playerLinearDrag_ = CONST(double, "PLAYER_LINEAR_DRAG");
	double playerAngularDrag_ = CONST(double, "PLAYER_ANGULAR_DRAG");
	//BananaPool bananaPool_;

public:
	PlayState(GameMode* gMode, string tmap);
	~PlayState();
	virtual void init();
	virtual void update();
	void createDeadBodies();
	virtual void render();
	virtual void handleInput();
	EntityManager* getEntityManager() const { return entityManager_; }
	b2World* getPhysicsWorld() const { return physicsWorld_; }
};
