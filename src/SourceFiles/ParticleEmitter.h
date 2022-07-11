#pragma once
#include "Component.h"
#include "Vector2D.h"
#include <queue>
#include <list>
#include "checkML.h"

class Collider;

class ParticleEmitter :
	public Component
{
protected:
	Collider* collider_ = nullptr;

	Vector2D position_;
	Vector2D direction_;
	Vector2D offset_ = Vector2D(0, 0);

	bool emitting_ = false;

	int textureId_;
	Texture* texture_ = nullptr;

	//Velocidad con las que salen las particulas
	float speed_;

	//Aleatoridad en la generaci�n de las part�culas
	int speedVariation_;
	int generationOdds_;
	int minGenerationOdds_;
	int maxGenerationOdds_;

	//Maximo angulo de aleatoridad segun el vector direccion
	//Valores entre [0,180]
	int emissionConeAngle_;

	//Tama�o de la particula en cuadrado
	Uint16 size_;

	//Tiempo que pasa viva la particula en milisegundos
	//default 1000 milisegundos
	Uint16 particleLifetime_;

	//Tiempo que pasa emitiendo en milisegundos
	//valor 0 para infinito
	//default 1000 milisegundos
	Uint16 emittingTime_;
	Uint16 timeEmitted_ = 0;
	Uint8 msPerFrame_ = 0;
	Uint8 maxParticles_;

	//Numero de texturas
	int numParticlesInSpriteSheet_;
	int textureSize_;

	struct Particle {
		Vector2D position;
		Vector2D direction;
		Uint16 lifeTime;
		float speed;
		int numTexture;
	};

	std::list<Particle> particles_;
	std::queue<std::list<Particle>::iterator> particlesToDelete_;
public:
	ParticleEmitter(Vector2D direction, int textureId, float speed, int numTextures = 1, int particleGenerationOdds = 5, Uint16 particleLifetime = 1000, Uint16 size = 20, Uint16 emittingTime = 1000, int speedVariation = 0, int emissionConeAngle = 10.0);
	virtual ~ParticleEmitter() { };

	virtual void init() override;
	virtual void update() override;
	virtual void draw() const override;
	void setOffset(Vector2D offset) { offset_ = offset; }

	//Devuelve si se ha detenido o empezado
	bool PlayStop() { timeEmitted_ = 0; return emitting_ = !emitting_; };

	void setMaxPaticles(Uint8 num) { maxParticles_ = num; };
	void setPositionCollider(Collider* col) { collider_ = col; };
	void setDirection(Vector2D dir) { direction_ = dir; };
	void setSize(Uint16 size) { size_ = size; };
	void setLifeTime(Uint16 time) { emittingTime_ = time; };
	void setMaxParticles(Uint8 max) { maxParticles_ = max; };
	void modifyGenerationOdds(int var);
	void setGenerationOddsClamp(int min, int max) { minGenerationOdds_ = min; maxGenerationOdds_ = max; }
};
