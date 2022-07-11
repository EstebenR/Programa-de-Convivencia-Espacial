#include "SDL_Game.h"
#include "Resources.h"
#include "InputHandler.h"
#include "PlayState.h"
#include "checkML.h"


unique_ptr<SDL_Game> SDL_Game::instance_;

SDL_Game::SDL_Game() {
	constants_ = Constants("../config/constants.json");
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_DisplayMode display;
	SDL_GetCurrentDisplayMode(0, &display);
	displayW_ = display.w;
	displayH_ = display.h;


	window_ = SDL_CreateWindow(constants_.getConstant<string>("WINDOW_NAME").c_str(), SDL_WINDOWPOS_CENTERED,
	                           SDL_WINDOWPOS_CENTERED, displayW_,
	                           displayH_, SDL_WINDOW_SHOWN);
	renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetLogicalSize(renderer_, constants_.getConstant<int>("WINDOW_WIDTH"),
	                         constants_.getConstant<int>("WINDOW_HEIGHT")); //renderer size independent from display

	initializeResources();
	switchFullscreen();

	MS_PER_FRAME_ = constants_.getConstant<double>("MS_PER_FRAME");

	audio_->setMusicVolume(constants_.getConstant<int>("MAX_MUSIC_VOLUME"));
	audio_->setChannelVolume(constants_.getConstant<int>("MAX_EFFECTS_VOLUME"), 0);
	inputHandler_ = new InputHandler();
	inputHandler_->initialiseGamepads();
}

SDL_Game::~SDL_Game() {
	closeResources();

	if (mpHost_ != nullptr)
		delete mpHost_;

	if(renderer_ != nullptr) {
		SDL_DestroyRenderer(renderer_);
	renderer_ = nullptr;
	}

	if(window_ != nullptr) {
		SDL_DestroyWindow(window_);
		window_ = nullptr;
	}

	SDL_Quit();
}

void SDL_Game::switchFullscreen()
{
	if(!isFullscreen_)
	{
		SDL_SetWindowSize(window_, displayW_, displayH_);
		SDL_SetWindowFullscreen(window_, SDL_WINDOW_FULLSCREEN);
		isFullscreen_ = true;
	}
	else
	{
		SDL_SetWindowSize(window_, displayW_*windowScale_, displayH_*windowScale_);
		SDL_SetWindowFullscreen(window_, 0);
		SDL_SetWindowPosition(window_, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);	
		isFullscreen_ = false;
	}
}

void SDL_Game::initializeResources() {
	srand(time(NULL));

	textures_ = new SDLTexturesManager();
	textures_->init();

	fonts_ = new SDLFontsManager();
	fonts_->init();

	audio_ = new SDLAudioManager();
	audio_->init();

	gamestateMachine_ = new GameStateMachine();

	for (auto& image : Resources::images_) {
		textures_->loadFromImg(image.id, renderer_, image.fileName, image.nHorizontalFrames, image.nVerticalFrames);
	}

	for (auto& font : Resources::fonts_) {
		fonts_->loadFont(font.id, font.fileName, font.size);
	}

	for (auto& txtmsg : Resources::messages_) {
		textures_->loadFromText(txtmsg.id, renderer_, txtmsg.msg,
			fonts_->getFont(txtmsg.fontId), txtmsg.color);
	}

	for (auto& sound : Resources::sounds_) {
		audio_->loadSound(sound.id, sound.fileName);
	}

	for (auto& music : Resources::musics_) {
		audio_->loadMusic(music.id, music.fileName);
	}
}

void SDL_Game::closeResources() {
	delete textures_;
	textures_ = nullptr;
	delete fonts_;
	fonts_ = nullptr;
	delete audio_;
	audio_ = nullptr;
	delete gamestateMachine_;
	gamestateMachine_ = nullptr;
	delete inputHandler_;
	inputHandler_ = nullptr;
}

void SDL_Game::start(char* addr, char* port)
{
	if(addr != nullptr) strcpy(addr_, addr);
	if(port != nullptr) strcpy(port_, port);

	exit_ = false;
	gamestateMachine_->changeToState(States::playableMenu, 0);
	//gamestateMachine_->changeToState(States::play, 4, GamemodeID::Timed, "BoilerRoom"); //BoilerRoom, LivingRoom, GymRoom

	//if (inputHandler_->getNumControllers() > 0) {
	while (!exit_) {
		Uint32 startTime = getTime();
		gamestateMachine_->gameCycle();
		Uint32 frameTime = getTime() - startTime;
		if (frameTime < MS_PER_FRAME_)
			SDL_Delay(MS_PER_FRAME_ - frameTime);
		
		if(startTime - lastSendTime >= MS_PER_SEND_TICK) {
			sendData_ = true;
			lastSendTime = startTime;
		}
		else sendData_ = false;
		//else cout << "LAGGING BEHIND! " << frameTime << endl << endl;

	}
	//}
	//else std::cout << "No hay mando conectado.\nAt SDL_Game.cpp line 113\n\n";
}
 
MultiplayerHost* SDL_Game::getHost() {
	if (mpHost_ == nullptr) {
		mpHost_ = new MultiplayerHost(addr_, port_);
		isHosting_ = true;
		gamestateMachine_->setMpHost(mpHost_);
	} return mpHost_;
}