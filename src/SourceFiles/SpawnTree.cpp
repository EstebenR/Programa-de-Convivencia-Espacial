#include "SpawnTree.h"
#include "Collider.h"

SpawnTree::SpawnTree(Texture* objectText, double objectW, double objectH, WeaponPool* pool, EntityManager* eMan, b2World* pWorld) :
	Component(ComponentType::SpawnTree), objectTex_(objectText), pool_(pool), eMan_(eMan), pWorld_(pWorld) {
	int pixPerMeter = CONST(double, "PIXELS_PER_METER");
	objectW_ = objectW * pixPerMeter;
	objectH_ = objectH * pixPerMeter;
}

void SpawnTree::init() {
	spawnTime_ = CONST(double, "OBJECT_SPAWN_TIME");
	pixPerM_ = CONST(double, "PIXELS_PER_METER");
	sPerFrame_ = CONST(double, "SECONDS_PER_FRAME");
	Collider* coll = GETCMP1_(Collider);
	SDL_Rect rectRender = coll->getRectRender();
	double spawnPosX = rectRender.x + (rectRender.w / 1.5);
	double spawnPosY = rectRender.y + objectH_ * 2;
	renderSpawnPos_ = b2Vec2(spawnPosX - (objectW_ * 0.5f), spawnPosY - (objectH_ * 0.5f));
	physicsSpawnPos_ = b2Vec2((spawnPosX / pixPerM_), (CONST(double, "WINDOW_HEIGHT") - spawnPosY) / pixPerM_);
	cout << renderSpawnPos_.x << " " << renderSpawnPos_.y << endl;
}

void SpawnTree::update() {
	timer_ += sPerFrame_;
	currentH_ = (objectH_ * timer_) / spawnTime_;
	currentW_ = (objectW_ * timer_) / spawnTime_;
	if (timer_ >= spawnTime_) {
		SpawnObject();
		timer_ = 0;
	}
}

void SpawnTree::draw() const {
	if (pool_->isRendering()) {
		SDL_Rect rect;
		rect.x = renderSpawnPos_.x; rect.y = renderSpawnPos_.y;
		rect.w = currentW_; rect.h = currentH_;
		objectTex_->render(rect);
	}
}

void SpawnTree::SpawnObject() {
	pool_->addWeapon(physicsSpawnPos_);
}