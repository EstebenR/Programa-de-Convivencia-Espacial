#pragma once

#include <string>
#include "checkML.h"
#include "json.hpp"
#include "checkML.h"
#include "Packets.h"

using json = nlohmann::json;

class Constants {
private:
	json data;  //coleccion de datos almacenados como json
	bool initialized_ = false;
public:
	Constants() {};
	Constants(const std::string& load);
	template<typename T>
	T getConstant(const std::string& key) const; //devuelve la constante pedida o su valor por defecto
};

//Mathz
const double PI = 3.14159265358979323846264338327950288;
const int FRAMES_PER_SECOND = 60;

//Pools
const int COIN_POOL_SIZE = 140;
const int FIREBALL_POOL_SIZE = 5;
const int TOMATO_POOL_SIZE = 10;
const int BANANA_POOL_SIZE = 5;
const int CONFETTI_POOL_SIZE = 5;
const int STAPLER_POOL_SIZE = 5;
const int BODY_POOL_SIZE = 5;

//Estados de juego
enum States
{
	menu,
	play,
	pause,
	midGame,
	transition,
	lobby,
	onlineMenu,
	client,
	playableMenu,
	credits,
	endGame,
	options,
	//No toques
	NUMBER_OF_STATES
};

enum GamemodeID {
	Capitalism,
	Controller,
	Stocks,
	WiFight,
	Timed,
	Tutorial,
	NUMBER_OF_GAMEMODES
};

enum WeaponID {
	NoWeapon,
	Piedra,
	Pelota,
	Mancuerna,
	Stapler,
	Extinguisher,
	Slipper,
	Mando,
	Tomato,
	Banana,
	Confetti,

	WEAPON_NUMBER
};

// no s� por qu� peta al hacer un array de strings jorge suspenso
/*std::string mapNames[] = {
	"LivingRoom",
	"Gym",
	"BoilerRoom"
};

enum MapID {
	LivingRoom,
	Gym,
	BoilerRoom,
	//Menu,
	//Garden,
	//Kitchen,
	//Entrance,

	// No toques
	NUMBER_OF_MAPS
};*/