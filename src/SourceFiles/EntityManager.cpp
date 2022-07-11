#include "EntityManager.h"
#include "Entity.h"
#include "Weapon.h"
#include "Component.h"

EntityManager::EntityManager()
{
	for(int i = 0; i<CONST(int,"Z_LAYERS");i++)
	{
		entities_.push_back(std::vector<std::unique_ptr<Entity>>());
	}
}

EntityManager::~EntityManager() {
	entities_.clear();
	weapons_.clear();
	externalEntities_.clear();
}

void EntityManager::update() {
	for (auto& v: entities_)
	{
		for (auto& e : v)
			if (e->isActive()) e->update();
	}
	for (auto& e : externalEntities_)
		if (e->isActive()) e->update();
}

void EntityManager::render() {
	for (auto& v: entities_)
	{
		for (auto& e : v)
			if (e->isActive()) e->render();
	}
	for (auto& e : externalEntities_)
		if (e->isActive()) e->render();
}
void EntityManager::handleInput() {
	for(auto& v: entities_)
		for (auto& e : v)
			if (e->isActive()) e->handleInput();
	for (auto& e : externalEntities_)
		if (e->isActive()) e->handleInput();
}

Entity* EntityManager::addEntity(uint8_t z) {
	Entity* e = new Entity(this);
	//e->setZIndex(z);
	std::unique_ptr<Entity> uPtr(e);
	entities_[z].emplace_back(std::move(uPtr));
	return e;
}

void EntityManager::addExistingEntity(Entity* e) {
	e->setEntityManager(this);
	externalEntities_.push_back(e);
}

void EntityManager::moveEntitieZ(Entity* e, const uint8_t& newZ)
{
	const uint8_t originZ = e->getZIndex();
	auto it = std::find_if(entities_[originZ].begin(), entities_[originZ].end(), [&](std::unique_ptr<Entity>& p)
    {
        return p.get() == e;
    });
    if (it != entities_[originZ].end())
    {
    	entities_[originZ].push_back( std::move(*it) );
        entities_[originZ].erase(it);
    }
}

std::vector<Weapon*>* EntityManager::getWeaponVector() { return &weapons_; }
