#include "SDLAudioManager.h"

#include <iostream>
#include <assert.h>
#include "SDL_Game.h"

using namespace std;

SDLAudioManager::SDLAudioManager() :
	SDLAudioManager(8) {
}

SDLAudioManager::SDLAudioManager(int channels) :
	initialized_(false), channels_(channels) {
}

SDLAudioManager::~SDLAudioManager() {
	if (!initialized_)
		return;

	// free all sound effect chucks
	for (const auto& pair : chunks_) {
		if (pair.second != nullptr)
			Mix_FreeChunk(pair.second);
	}

	// free all music sound effect
	for (const auto& pair : music_) {
		if (pair.second != nullptr)
			Mix_FreeMusic(pair.second);
	}

	// close SDL_Mixer
	Mix_Quit();
}

bool SDLAudioManager::init() {
	if (initialized_)
		return false;

	// open the mixer
	int mixOpenAudio = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	assert(mixOpenAudio == 0);

	// initialize supported formats
	int mixInit_ret = Mix_Init(
		MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG);
	assert(mixInit_ret != 0);

	// set number of channels
	channels_ = Mix_AllocateChannels(channels_);

	initialized_ = true;

	return true;
}

bool SDLAudioManager::loadSound(int tag, const string& fileName) {
	if (!initialized_)
		return false;

	Mix_Chunk* chunk = Mix_LoadWAV(fileName.c_str());
	if (chunk != nullptr) {
		Mix_Chunk* curr = chunks_[tag];
		if (curr != nullptr)
			Mix_FreeChunk(curr);
		chunks_[tag] = chunk;
		return true;
	}
	else {
		throw "Couldn't load sound file: " + fileName;
		return false;
	}
}

int SDLAudioManager::playChannel(int tag, int loops, int channel) {
	Mix_Chunk* chunk = chunks_[tag];
	if (chunk != nullptr)
	{
		if (SDL_Game::instance()->haveToSend()) {
			AudioPacket aP = { false, (char)tag, (char)loops };
			SDL_Game::instance()->getHost()->addAudio(aP);
		}
		return Mix_PlayChannel(channel, chunk, loops);
	}
	else {
		return -1;
	}
}

void SDLAudioManager::pauseChannel(int channel) {
	Mix_Pause(channel);
}

void SDLAudioManager::resumeChannel(int channel) {
	Mix_Resume(channel);
}

void SDLAudioManager::haltChannel(int channel) {
	Mix_HaltChannel(channel);
}

int SDLAudioManager::setChannelVolume(int volume, int channel) {
	return Mix_Volume(channel, volume);
}

int SDLAudioManager::channels() {
	return channels_;
}

bool SDLAudioManager::loadMusic(int tag, const string& fileName) {
	if (!initialized_)
		return false;

	Mix_Music* music = Mix_LoadMUS(fileName.c_str());
	if (music != nullptr) {
		Mix_Music* curr = music_[tag];
		if (curr != nullptr)
			Mix_FreeMusic(curr);
		music_[tag] = music;
		return true;
	}
	else {
		throw "Couldn't load music file: " + fileName;
		return false;
	}
}

void SDLAudioManager::playMusic(int tag, int loops) {
	Mix_Music* music = music_[tag];
	if (music != nullptr) {
		Mix_PlayMusic(music, loops);

		if (SDL_Game::instance()->isHosting()) {
			AudioPacket aP = { true, (char)tag, (char)loops };
			SDL_Game::instance()->getHost()->addAudio(aP);
		}
	}
}

int SDLAudioManager::setMusicVolume(int volume) {
	return Mix_VolumeMusic(volume);
}

void SDLAudioManager::haltMusic() {
	Mix_HaltMusic();
}

void SDLAudioManager::pauseMusic() {
	Mix_PauseMusic();
}

void SDLAudioManager::resumeMusic() {
	Mix_ResumeMusic();
}