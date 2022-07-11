#include "ParticleEmitter.h"
#include "Entity.h"
#include "Collider.h"

ParticleEmitter::ParticleEmitter(Vector2D direction, int textureId, float speed, int numTextures, int generationOdds, Uint16 particleLifetime, Uint16 size, Uint16 emittingTime, int speedVariation, int emissionConeAngle) :
	Component(ComponentType::ParticleEmitter), direction_(direction), textureId_(textureId), speed_(speed), numParticlesInSpriteSheet_(numTextures), generationOdds_(generationOdds),
	particleLifetime_(particleLifetime), size_(size), emittingTime_(emittingTime), speedVariation_(speedVariation), emissionConeAngle_(emissionConeAngle),
	msPerFrame_(0), maxParticles_(0) {}

void ParticleEmitter::init() {
	collider_ = GETCMP1_(Collider);
	texture_ = SDL_Game::instance()->getTexturesMngr()->getTexture(textureId_);
	textureSize_ = texture_->getHeight(); //Asumimos que si es un spritesheet, est�n organizados en vertical.

	//Load of constants
	msPerFrame_ = CONST(float, "MS_PER_FRAME");
	maxParticles_ = CONST(int, "MAX_PARTICLES_DEFAULT");
	minGenerationOdds_ = CONST(int, "PARTICLE_GENERATION_MIN_ODDS");
	maxGenerationOdds_ = CONST(int, "PARTICLE_GENERATION_MAX_ODDS");
}

void ParticleEmitter::update() {
	if (emitting_) {
		timeEmitted_ += msPerFrame_;
		if (emittingTime_ != 0) {
			if (timeEmitted_ > emittingTime_)
				emitting_ = false;
		}
		if (particles_.size() < maxParticles_ && timeEmitted_ % generationOdds_ == 0) {
			SDL_Rect colliderRect = collider_->getRectRender();
			//direction + angulo random
			int angle = rand() % int(emissionConeAngle_ * 2) - emissionConeAngle_;
			Vector2D direction = direction_.rotate(angle);
			//speed_ + variacion random
			float speed = (speedVariation_ != 0) ? speed_ - speedVariation_ + rand() % int(speedVariation_ * 2) : speed_;
			//frame
			int numTexture = 0;
			if (numParticlesInSpriteSheet_ > 0) numTexture = rand() % numParticlesInSpriteSheet_;

			particles_.push_back({ Vector2D((double)colliderRect.x + colliderRect.w * 0.5f + offset_.getX(),(double)colliderRect.y + colliderRect.h * 0.5f + offset_.getY()),direction,0,speed, numTexture });
		}
	}

	if (particles_.size() > 0) {
		for (list<Particle>::iterator part = particles_.begin(); part != particles_.end(); ++part) {
			part->lifeTime += msPerFrame_;
			if (part->lifeTime > particleLifetime_)
				particlesToDelete_.push(part);
			else
				part->position = part->position + part->direction * part->speed;
		}
		while (particlesToDelete_.size() > 0) {
			particles_.erase(particlesToDelete_.front());
			particlesToDelete_.pop();
		}
	}
}

void ParticleEmitter::draw() const {
	for (Particle part : particles_) {
		texture_->render({ (int)((int)part.position.getX() - size_ * 0.5f),(int)((int)part.position.getY() - size_ * 0.5f),size_,size_ }, 0, part.numTexture); //part numTexture es el frame
	}
}

void ParticleEmitter::modifyGenerationOdds(int var)
{
	generationOdds_ += var;
	if (generationOdds_ < minGenerationOdds_) generationOdds_ = minGenerationOdds_;
	else if (generationOdds_ > maxGenerationOdds_) generationOdds_ = maxGenerationOdds_;
}