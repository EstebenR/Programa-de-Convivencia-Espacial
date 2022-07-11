#ifndef SDL_GAME_H
#define SDL_GAME_H

#include <SDL.h>
#include "Constants.h"
#include <memory>
#include <assert.h>

#include "EntityManager.h"
#include "SDLTexturesManager.h"
#include "SDLAudioManager.h"
#include "SDLFontsManager.h"
#include "GameStateMachine.h"
#include "MultiplayerHost.h"

#include "SDL_macros.h"
#include "iostream"
#include "checkML.h"


using namespace std;
//Interfaz entre el juego y el display
class InputHandler;
class Constants;
#define CONST(type,key) SDL_Game::instance()->getConstants()->getConstant<type>(key)
class SDL_Game
{
protected:

	SDLFontsManager* fonts_ = nullptr;
	SDLTexturesManager* textures_ = nullptr;
	SDLAudioManager* audio_ = nullptr;
	GameStateMachine* gamestateMachine_ = nullptr;
	SDL_Window* window_ = nullptr;
	SDL_Renderer* renderer_ = nullptr;
	InputHandler* inputHandler_ = nullptr;
	MultiplayerHost* mpHost_ = nullptr;
	Constants constants_;

	double MS_PER_FRAME_;

	static unique_ptr<SDL_Game> instance_;

	bool sendData_ = false;
	bool isHosting_ = false;
	bool isFullscreen_ = false;
	int displayW_=1920;
	int displayH_=1080;
	float windowScale_ = 0.75;

private:
	bool exit_;
	void initializeResources();
	void closeResources();

	Uint32 lastSendTime = 0;
	Uint32 MS_PER_SEND_TICK = 60;

	// 6 porque 65535 es el tamaño max de puerto (2^16)
	char addr_[108] = "localhost", port_[6] = "2000";

public:
	void start(char* addr = nullptr, char* port = nullptr);
	SDL_Game();
	virtual ~SDL_Game();

	SDL_Game(SDL_Game&) = delete;
	SDL_Game& operator=(SDL_Game&) = delete;

	inline static SDL_Game* instance() {
		if (instance_.get() == nullptr) {
			instance_.reset(new SDL_Game());
		}
		return instance_.get();
	}

	char* getAddr() {return addr_;}
	char* getPort() {return port_;}

	inline unsigned int getTime() {
		return SDL_GetTicks();
	}

	inline void exitGame() { exit_ = true; }
	inline bool isExit() const { return exit_; }

	inline void switchFullscreen();

	InputHandler* getInputHandler() { return inputHandler_; }

	SDLTexturesManager* getTexturesMngr() { return textures_; }
	SDLAudioManager* getAudioMngr() { return audio_; }
	SDLFontsManager* getFontMngr() { return fonts_; }
	GameStateMachine* getStateMachine() { return gamestateMachine_; }
	SDL_Renderer* getRenderer() { return  renderer_; }
	const Constants* getConstants() { return &constants_; }

	double getMS_PER_FRAME() const { return MS_PER_FRAME_; }

	bool isHosting() const {
		return isHosting_;
	}

	bool haveToSend() const {
		return isHosting_ && sendData_;
	}

	MultiplayerHost* getHost();
};

#endif //SDL_GAME_H