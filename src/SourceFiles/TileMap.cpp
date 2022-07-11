#include "TileMap.h"
#include "SDL_Game.h"
#include "Resources.h"
#include "json.hpp"
#include "BoilerButtonLogic.h"
#include "FireBallGenerator.h"
#include "ConfettiPool.h"
#include <string>
#include "TomatoPool.h"
#include "BananaPool.h"
#include "GameMode.h"
#include "WeaponFactory.h"

TileMap::TileMap(int w, int h, string map, EntityManager *eM, b2World *pW, BulletPool *bp, ConfettiPool *cP, StaplerPool *staplerPool, GameMode *gameMode) : Component(ComponentType::Tilemap), //w y h son de la ventana
	width_(w),
	height_(h),
	entityManager_(eM),
	physicsWorld_(pW),
	bulletPool_(bp),
	confettiPool_(cP),
	staplerPool_(staplerPool),
	gameMode_(gameMode)
{
	loadTileson(map);
	playerSpawns_.reserve(4);
	for (int i = 0; i < 4; i++)
	{ //inicializa el vector
		playerSpawns_.push_back(b2Vec2());
	}
}

TileMap::~TileMap()
{
}

void TileMap::init()
{
	layers_ = tMap_.getLayers();
	//recorremos todas las capas del mapa
	for (auto &tileLayer : layers_)
	{
		//podemos distinguir entre capas de tiles, objetos, imagenes m�s adelante
		if (tileLayer.getType() == tson::Layer::Type::ObjectGroup)
		{
			//pos = position in tile units
			vector<tson::Object> objetos = tileLayer.getObjects();
			//std::cout << "NumItems capa " << tileLayer.getName() << ": " << objetos.size() << "\n";
			for (auto &obj : objetos)
			{
				if (tileLayer.getName() == "Walls")
				{ //muros
					factoryItems_.push_back(obj);
				}
				else if (tileLayer.getName() == "Spawns")
				{ //spawns
					if (obj.getName() == "P1")
						playerSpawns_[0] = b2Vec2(obj.getPosition().x / CONST(double, "PIXELS_PER_METER"), (CONST(int, "WINDOW_HEIGHT") - obj.getPosition().y) / CONST(double, "PIXELS_PER_METER"));
					else if (obj.getName() == "P2")
						playerSpawns_[1] = b2Vec2(obj.getPosition().x / CONST(double, "PIXELS_PER_METER"), (CONST(int, "WINDOW_HEIGHT") - obj.getPosition().y) / CONST(double, "PIXELS_PER_METER"));
					else if (obj.getName() == "P3")
						playerSpawns_[2] = b2Vec2(obj.getPosition().x / CONST(double, "PIXELS_PER_METER"), (CONST(int, "WINDOW_HEIGHT") - obj.getPosition().y) / CONST(double, "PIXELS_PER_METER"));
					else if (obj.getName() == "P4")
						playerSpawns_[3] = b2Vec2(obj.getPosition().x / CONST(double, "PIXELS_PER_METER"), (CONST(int, "WINDOW_HEIGHT") - obj.getPosition().y) / CONST(double, "PIXELS_PER_METER"));
				}
				else if (tileLayer.getName() == "SpecialObjects")
				{ //objetos espaciales (mando de tele, router...)
					specialObjectsSpawnPoint_ = b2Vec2(obj.getPosition().x / CONST(double, "PIXELS_PER_METER"), (CONST(int, "WINDOW_HEIGHT") - obj.getPosition().y) / CONST(double, "PIXELS_PER_METER"));
				}
				else if (tileLayer.getName() == "MapObjects")
				{ //muebles
					factoryItems_.push_back(obj);
				}
				else if (tileLayer.getName() == "LowTierWeapons")
				{
					lowTierWeaponSpawnPoints_.push_back(b2Vec2(obj.getPosition().x / CONST(double, "PIXELS_PER_METER"), (CONST(int, "WINDOW_HEIGHT") - obj.getPosition().y) / CONST(double, "PIXELS_PER_METER")));
				}
				else if (tileLayer.getName() == "MidTierWeapons")
				{
					midTierWeaponSpawnPoints_.push_back(b2Vec2(obj.getPosition().x / CONST(double, "PIXELS_PER_METER"), (CONST(int, "WINDOW_HEIGHT") - obj.getPosition().y) / CONST(double, "PIXELS_PER_METER")));
				}
				else if (tileLayer.getName() == "HighTierWeapons")
				{
					highTierWeaponSpawnPoints_.push_back(b2Vec2(obj.getPosition().x / CONST(double, "PIXELS_PER_METER"), (CONST(int, "WINDOW_HEIGHT") - obj.getPosition().y) / CONST(double, "PIXELS_PER_METER")));
				}
				else if (tileLayer.getName() == "CoinSpawners")
				{
					coinsSpawnPoints_.push_back(b2Vec2(obj.getPosition().x / CONST(double, "PIXELS_PER_METER"), (CONST(int, "WINDOW_HEIGHT") - obj.getPosition().y) / CONST(double, "PIXELS_PER_METER")));
				}
				else
					std::cout << "Not recognized " << tileLayer.getName() << "\n";
			}
		}
	}
	if (gameMode_ != nullptr)
		bulletPool_->init(entityManager_, physicsWorld_, gameMode_);
}

void TileMap::update()
{
	//apa�o para que draw sea const
	layers_ = tMap_.getLayers();
}

void TileMap::draw() const
{
	//c�digo bastante modificado basado en https://github.com/SSBMTonberry/tileson
	//He a�adido sistema de escalado
	//soporte autom�tico para varios tilesets
	//soporte de texturas de SDL

	Texture *tilesetT_;		   //Textura del tileset a utilizar
	const tson::Tileset *tSet; //Datos del tileset a utilizar

	//recorremos todas las capas del mapa
	for (auto &tileLayer : layers_)
	{
		//podemos distinguir entre capas de tiles, objetos, imagenes m�s adelante
		if (tileLayer.getType() == tson::Layer::Type::TileLayer)
		{
			//pos = position in tile units
			for (auto &[pos, tile] : tileLayer.getTileData()) //Loops through absolutely all existing tiles
			{
				if (tile != nullptr)
				{
					//buscamos el tileset correspondiente
					int i = 0;
					int id = tile->getId();
					while (i < tileSets_.size() && !(id >= tileSets_[i].getFirstgid() && id <= (tileSets_[i].getFirstgid() + tileSets_[i].getTileCount()) - 1))
					{
						i++;
					}
					if (i < tileSets_.size())
						tSet = &tileSets_[i];
					else
					{
						throw std::runtime_error("No se encontro el tileset");
					}

					//variables auxiliares para el dibujado del tile
					int firstId = tSet->getFirstgid(); //First tile id of the tileset
					int tSetColumns = tSet->getColumns();
					int tSetRows = tSet->getTileCount() / tSetColumns;
					int lastId = (tSet->getFirstgid() + tSet->getTileCount()) - 1;
					int tileWidth = tSet->getTileSize().x;
					int tileHeight = tSet->getTileSize().y;
					int spacing = tSet->getSpacing();
					int margin = tSet->getMargin();

					tilesetT_ = SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::tilesetTag_.find(tSet->getName())->second);
					
					//Posicion de dibujado del vector
					tson::Vector2i position = {std::get<0>(pos) * width_ / mapCols_, std::get<1>(pos) * height_ / mapRows_};

					//posicion unidimensional del tile en el tileset
					int baseTilePosition = (tile->getId() - firstId); //This will determine the base position of the tile.

					//fila y columna del tile en el tileset
					int currentCol = (baseTilePosition % tSetColumns);
					int currentRow = (baseTilePosition / tSetRows);

					//posiciones del tile en el tileset
					int offsetX = currentCol * (tileWidth + spacing);
					int offsetY = (currentRow) * (tileHeight + margin);

					SDL_Rect drawPos = {position.x, position.y, width_ / mapCols_, height_ / mapRows_};
					SDL_Rect tilesetClip = {offsetX, offsetY, tileWidth, tileWidth};
					tilesetT_->render(drawPos, tilesetClip);
				}
				std::cout << "Tile ";
			}
		}
		else
			std::cout << "No es el tipo que querias\n";
	}
}

bool TileMap::loadTileson(string path)
{
	try
	{
		tson::Tileson parser;
		tMap_ = parser.parse(fs::path(path));
		if (tMap_.getStatus() == tson::Map::ParseStatus::OK)
		{
			//variables de escala
			mapCols_ = tMap_.getSize().x;
			mapRows_ = tMap_.getSize().y;
			//guardamos los datos de los tilesets que se usan
			tileSets_ = tMap_.getTilesets();
			//std::cout << tMap_.getSize().x << " " << tMap_.getSize().y << " " << tMap_.getTilesets().size() << "\n";
			return true;
		}
		std::cout << "No se pudo cargar el mapa\n";
		return false;
	}
	catch (...)
	{
		throw std::runtime_error("No se cargó el mapa / parse");
	}
}

void TileMap::executeMapFactory()
{
	for (auto o : factoryItems_)
	{
		tson::Vector2i p, s;
		s = o.getSize();
		p = o.getPosition();
		b2Vec2 size;
		//Calculo de posicion para puntos de Tiled (la mayoria de items)
		b2Vec2 pos = b2Vec2(p.x / CONST(double, "PIXELS_PER_METER"), (CONST(int, "WINDOW_HEIGHT") - p.y) / CONST(double, "PIXELS_PER_METER"));
		string name = o.getName();

		if (name == "Wall")
		{
			//calculo de posicion y tama�o para cajas de Tiled
			size = b2Vec2(s.x / CONST(double, "PIXELS_PER_METER"), s.y / CONST(double, "PIXELS_PER_METER"));
			pos = b2Vec2(pos.x + (size.x * 0.5f), pos.y - (size.y * 0.5f));
			size *= 0.5f;
			ObjectFactory::makeWall(entityManager_, physicsWorld_, pos, size);
		}
		else if (name == "SpaceJunk")
		{
			ObjectFactory::makeSpaceJunk(entityManager_, physicsWorld_, pos, b2Vec2(0.5, 0.5));
		}
		else if (name == "Table")
		{
			ObjectFactory::makeTable(entityManager_, physicsWorld_, pos, b2Vec2(CONST(float, "TABLE_W_PHYSICS"), CONST(float, "TABLE_H_PHYSICS")));
		}
		else if (name == "Lamp")
		{
			ObjectFactory::makeLamp(entityManager_, physicsWorld_, pos, b2Vec2(CONST(float, "LAMP_W_PHYSICS"), CONST(float, "LAMP_H_PHYSICS")));
		}
		else if (name == "Sofa")
		{
			ObjectFactory::makeSofa(entityManager_, physicsWorld_, pos, b2Vec2(CONST(float, "SOFA_W_PHYSICS"), CONST(float, "SOFA_H_PHYSICS")));
		}
		else if (name == "Pad")
		{
			size = b2Vec2(s.x / CONST(double, "PIXELS_PER_METER"), (s.y) / CONST(double, "PIXELS_PER_METER"));
			pos = b2Vec2(pos.x + (size.x * 0.5f), pos.y - (size.y * 0.5f));
			size *= 0.5f;
			ObjectFactory::makePad(entityManager_, physicsWorld_, pos, size);
		}
		else if (name == "Boiler")
		{
			boilerAux_ = GETCMP2(ObjectFactory::makeBoiler(entityManager_, physicsWorld_, pos), FireBallGenerator);
		}
		else if (name == "IncButton")
		{
			boilerButtons_.push_back(GETCMP2(ObjectFactory::makeBoilerButton(entityManager_, physicsWorld_, pos, true), BoilerButtonLogic));
		}
		else if (name == "DecButton")
		{
			boilerButtons_.push_back(GETCMP2(ObjectFactory::makeBoilerButton(entityManager_, physicsWorld_, pos, false), BoilerButtonLogic));
		}
		else if (name == "Pipe")
		{
			float rotation = o.getRotation();
			size = b2Vec2(s.x / CONST(double, "PIXELS_PER_METER"), s.y / CONST(double, "PIXELS_PER_METER"));
			pos = b2Vec2(pos.x + (size.x * 0.5f), pos.y - (size.y * 0.5f));
			size *= 0.5f;

			ObjectFactory::makePipe(entityManager_, physicsWorld_, pos, size, rotation);
		}
		else if (name == "Treadmill")
		{
			ObjectFactory::makeTreadmill(entityManager_, physicsWorld_, pos);
		}
		else if (name == "CarnivorousePlant")
		{
			size = b2Vec2(s.x / CONST(double, "PIXELS_PER_METER"), (s.y) / CONST(double, "PIXELS_PER_METER"));
			pos = b2Vec2(pos.x + (size.x * 0.5f), pos.y - (size.y * 0.5f));
			size *= 0.5f;
			ObjectFactory::makeCarnivorousPlant(entityManager_, physicsWorld_, pos, size);
		}
		else if (name == "TomatoTree")
		{
			if (tomatoPool_ == nullptr)
			{
				tomatoPool_ = make_unique<TomatoPool>();
				tomatoPool_->init(entityManager_, physicsWorld_);
			}
			ObjectFactory::makeTomatoTree(entityManager_, physicsWorld_, pos, tomatoPool_.get());
		}
		else if (name == "BananaTree")
		{
			if (bananaPool_ == nullptr)
			{
				bananaPool_ = make_unique<BananaPool>(bulletPool_);
				bananaPool_->init(entityManager_, physicsWorld_);
			}
			ObjectFactory::makeBananaTree(entityManager_, physicsWorld_, pos, bananaPool_.get());
		}
		else if (name == "TriggerButton")
		{
			size = b2Vec2(s.x / CONST(double, "PIXELS_PER_METER"), (s.y) / CONST(double, "PIXELS_PER_METER"));
			pos = b2Vec2(pos.x + (size.x * 0.5f), pos.y - (size.y * 0.5f));
			size *= 0.5f;
			string buttonType = o.getType();
			; //Por defecto

			doors_.push_back(ObjectFactory::makeTriggerButton(entityManager_, physicsWorld_, pos, size, buttonType)->getComponent<AnimatedViewer>(ComponentType::Viewer));
		}

		else if (name == "TWall")
		{
			float rotation = o.getRotation();
			size = b2Vec2(s.x / CONST(double, "PIXELS_PER_METER"), s.y / CONST(double, "PIXELS_PER_METER"));
			pos = b2Vec2(pos.x + (size.x * 0.5f), pos.y - (size.y * 0.5f));
			size *= 0.5f;

			ObjectFactory::makeTrasparentWall(entityManager_, physicsWorld_, pos, size, rotation);
		}

		else
			std::cout << "Objeto no reconocido\n";
	}
	solvePostCreationProblems();
}

void TileMap::solvePostCreationProblems()
{
	if (boilerAux_)
	{
		for (int k = 0; k < boilerButtons_.size(); k++)
		{
			boilerButtons_[k]->assignBoiler(boilerAux_);
		}
	}
	else if (!boilerAux_ && boilerButtons_.size() > 0)
		cout << "WARNING: Can't create buttons without a boiler. On TileMap.cpp - line 190" << endl;
}

b2Vec2 TileMap::getPlayerSpawnPoint(int id)
{
	if (id < playerSpawns_.size())
		return playerSpawns_[id];
	else
		return b2Vec2();
}

void TileMap::createWeapons()
{
	for (b2Vec2 spawnPoint : midTierWeaponSpawnPoints_)
	{ //recorre todos los spawn
		WeaponFactory::makeMidTierWeapon(entityManager_, physicsWorld_, spawnPoint);
	}

	for (b2Vec2 spawnPoint : highTierWeaponSpawnPoints_)
	{
		WeaponFactory::makeHighTierWeapon(entityManager_, physicsWorld_, spawnPoint);
	}

	for (b2Vec2 spawnPoint : lowTierWeaponSpawnPoints_)
	{
		ObjectFactory::makeWeaponSpawner(entityManager_, physicsWorld_, spawnPoint, confettiPool_, staplerPool_, bulletPool_);
	}
}