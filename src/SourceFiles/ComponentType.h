#pragma once
#include <ctime>
#include "checkML.h"


namespace ComponentType {
	using CmpIdType = std::size_t;

	enum CmpId : CmpIdType {
		//Vayan metiendo aqui sus componentes segun lo hagan
		Transform = 0,
		Health,
		Viewer,
		Collider,
		AttachesToObjects,
		HealthViewer,
		Tilemap,
		Weapon,
		Hands,
		PlayerController,
		ImpulseViewer,
		PlayerData,
		RouterLogic,
		Wallet,
		Coin,
		ParticleEmitter,
		FireBallGenerator,
		Fireball,
		BoilerButtonLogic,
		Pad,
		ThrownByPlayer,
		Treadmill,
		CarnivorousPlant,
		AnimatedViewer,
		AnimatedUIViewer,
		UIViewer,
		AdvancedAnimatedViewer,
		Bullet,
		RocketLogic,
		Countdown,
		SpawnTree,
		TriggerButton,
		TimedDespawn,
		WeaponSpawner,

		// don't touch the rest of this enum, it is to identify the max
		// number of components
		_LastCmptId_
	};

	constexpr std::size_t maxComponents = _LastCmptId_;

	// these two should be used to get a component via the field
	// entity_
#define GETCMP2_(id,type)  GETCMP3(entity_,id,type)
#define GETCMP1_(type)     GETCMP3(entity_,ComponentType::type,type)
	// this one should be used to get a component via a fixture
#define GETCMP_FROM_FIXTURE_(fixture, type) static_cast<type*>(GETCMP3(static_cast<Entity*>((void*)fixture->GetBody()->GetUserData().pointer),ComponentType::type,type))

// these two should be used to get a component via an
// entity e provided as a parameter
#define GETCMP2(e,type) GETCMP3(e,ComponentType::type,type)
#define GETCMP3(e,id,type) e->getComponent<type>(id)
}
