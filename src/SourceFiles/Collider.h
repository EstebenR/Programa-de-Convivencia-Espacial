#pragma once
#include "Component.h"
#include "box2d.h"
#include "SDL_rect.h"
#include "Constants.h"
#include "checkML.h"


class Collider : public Component
{
public:
	//Add the different collision layers as we see fit
	enum CollisionLayer : uint16_t {
		//a collision layer can't be zero or else it won't collide
		NormalObject = 0x0001,				//	= 0000 0000 0000 0001
		NormalAttachableObject = 0x0002,	//	= 0000 0000 0000 0010
		UnInteractableObject = 0x0004,		//	= 0000 0000 0000 0100
		Wall = 0x0008,						//	= 0000 0000 0000 1000
		Trigger = 0x0010,					//	= 0000 0000 0001 0000
		PickableObject = 0x0020,			//	= 0000 0000 0010 0000
		NonGrababbleWall = 0x0040,			//	= 0000 0000 0100 0000
		Player1 = 0x0080,					//	= 0000 0000 1000 0000
		Player2 = 0x0100,					//	= 0000 0001 0000 0000
		Player3 = 0x0200,					//	= 0000 0010 0000 0000
		Player4 = 0x0400,					//	= 0000 0100 0000 0000

		// layers que resumen juntando varias capas en una
		Player = 0x0780,					//	= 0000 0111 1000 0000
		InteractsWithPlayer = 0x07FB		//  = 0000 0111 1111 1011
	};

private:
	double PIXELS_PER_METER;
	int WINDOW_HEIGHT;
	//general
	b2Body* body_;
	b2World* world_;
	b2BodyDef bodyDef_;

	//fixtures
	vector<float> widths_;
	vector<float> heights_;
	//vector<b2Shape> shapes_;
	vector<b2FixtureDef> fixtureDefs_;
	vector<b2Fixture*> fixtures_;

public:

	//Friccion -> rozamiento al contacto con otros cuerpos   Drag-> rozamiento con el aire
	Collider(b2World* world, b2BodyType type, float x, float y, float width, float height,
		float density, float friction, float restitution, float linearDrag, float angDrag, CollisionLayer c, bool sensor);

	Collider(b2World* world, b2BodyType type, float x, float y, float radius,
		float density, float friction, float restitution, float linearDrag, float angDrag, CollisionLayer c, bool sensor);

	~Collider() {
		//world_->DestroyBody(body_); world_ = nullptr;
		// el body se destruye autom�ticamente al destruir el world
	}

	virtual void init() override;

	//getters
	float getW(int i) const { return widths_[i]; }
	float getH(int i) const { return heights_[i]; }
	b2Body* getBody() { return body_; }
	b2Vec2 getPos() const { return body_->GetPosition(); }
	b2Vec2 getLinearVelocity() const { return body_->GetLinearVelocity(); }
	b2Body* getBody() const { return body_; }
	b2BodyType getType() const { return body_->GetType(); }
	b2Transform getTransform() const { return body_->GetTransform(); }
	b2World* getWorld() const { return world_; }
	b2Fixture* getFixture(int i) const { return fixtures_[i]; }
	float getAngle() const { return body_->GetAngle(); }
	float getAngleInDegrees() const { return body_->GetAngle() * -57.2957795131; }		// 57.2957795131 = 180/PI
	float getMass() const { return body_->GetMass(); }
	bool isAwake() const { return body_->IsAwake(); }
	bool isSensor(int i) const { return fixtureDefs_[i].isSensor; }
	bool isEnabled() const { return body_->IsEnabled(); }
	bool isBullet() const { return body_->IsBullet(); }
	int getNumFixtures() const { return fixtures_.size(); }
	SDL_Rect getRect() const {
		return SDL_Rect{
			(int)getPos().x, (int)getPos().y,
			(int)getW(0), (int)getH(0)
		};
	}

	SDL_Rect getRectRender(int index = 0) const {
		return SDL_Rect{
			(int)(getPos().x * PIXELS_PER_METER - (getW(index) * PIXELS_PER_METER)),
			(int)(WINDOW_HEIGHT - (getPos().y * PIXELS_PER_METER + (getH(index) * PIXELS_PER_METER))),
			(int)(getW(index) * PIXELS_PER_METER * 2),
			(int)(getH(index) * PIXELS_PER_METER * 2)
		};
	}

	//setters
	void setType(b2BodyType t) { body_->SetType(t); }
	void setAwake(bool b) { body_->SetAwake(b); }
	void setEnabled(bool b) { body_->SetEnabled(b); }
	void setBullet(bool b) { body_->SetBullet(b); }
	void setUserData(void* data) { body_->GetUserData().pointer = (uintptr_t)data; }
	void setTransform(b2Vec2 pos, float angle) { body_->SetTransform(pos, angle); }
	void setLinearVelocity(b2Vec2 vel) { body_->SetLinearVelocity(vel); }
	void setAngularVelocity(float vel) { body_->SetAngularVelocity(vel); }
	void applyLinearImpulse(b2Vec2 force, b2Vec2 point) { body_->ApplyLinearImpulse(force, body_->GetPosition() + point, true); }
	void applyForce(b2Vec2 force, b2Vec2 point) { body_->ApplyForce(force, body_->GetPosition() + point, true); }
	void createRectangularFixture(float width, float height, float density,
		float friction, float restitution, CollisionLayer layer, bool sensor);
	void createCircularFixture(float radius, float density, float friction, float restitution, CollisionLayer layer, bool sensor);
	void destroyFixture(int i);
	void changeLayerCollision(int i, int layer) { //suma o resta un layer de la colision con un fixture. poned el layer negativo para restar
		b2Filter f = fixtures_[i]->GetFilterData();
		f.maskBits += layer;
		fixtures_[i]->SetFilterData(f);
	}

	b2Filter getFilterFromLayer(CollisionLayer c);
	b2Filter setFilterLayerBits(b2Filter filter);
	void disableFixtureCollisions(int FixtureIndex);
};
