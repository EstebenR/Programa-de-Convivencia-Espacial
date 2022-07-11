#include "ClientState.h"
#include "SDL_Game.h"
#include "Texture.h"
#include <iostream>

#define us unsigned short
#define uc unsigned char

ClientState::ClientState(const char* addr, const char* port) :
	socket(addr, port)
{
	MS_PER_FRAME = SDL_Game::instance()->getMS_PER_FRAME();

	if (!socket.correct()) {
		throw std::runtime_error("Client socket initialization failed");
	}
	socket.bind();

	buffer = (char *)malloc(Socket::MAX_MESSAGE_SIZE);
	spritesToRender_.reserve(200);
}

ClientState::~ClientState()
{
	delete buffer;
	delete rcvThread;
	SDLNet_Quit(); 
}

void ClientState::init() {
	playerInfoVector_ = SDL_Game::instance()->getStateMachine()->getMatchInfo()->getPlayersInfo();

    rcvThread = new std::thread(&ClientState::rcv, this);
}

void ClientState::update() {
	if(!connected_) {
		char aux = 'J';
		socket.send(&aux, socket, 1);
	}
}

void ClientState::rcv()
{
	while(!SDL_Game::instance()->isExit()) 
	{
		if(socket.recv(buffer) != -1)
		{
			char* aux = buffer;

			unsigned int frameId;
			memcpy(&frameId, aux, sizeof(unsigned int));
			aux += sizeof(unsigned int);

			if(frameId <= currentFrameId_) return;	// ya hemos recibido este paquete, lo descartamos

			currentFrameId_ = frameId;

			uint16_t len;
			memcpy(&len, aux, sizeof(uint16_t));
			aux += sizeof(uint16_t);

			// descartamos el paquete si el tamano no tiene sentido
			if(len >= Socket::MAX_MESSAGE_SIZE || len < sizeof(unsigned int) + sizeof(uint16_t)) {
				std::cout << "Paquete corrupto.\n";
				return; 
			}

			// comprobar secuencia de seguridad
			// uint8_t securitySequence;
			// memcpy(&securitySequence, aux, sizeof(uint8_t));
			// aux += sizeof(uint8_t);
			// if(securitySequence != 0xFF) {
			// 	std::cout << "Paquete corrupto.\n";
			// 	return;
			// }

			// el paquete es correcto, hemos recibido un nuevo frame actualizado. Sincronizamos info y descartamos el frame actual
			lastUpdateInstant = SDL_Game::instance()->getTime();

			spriteMutex.lock();
			spritesToRender_.clear();
			spriteMutex.unlock();

			aux = buffer;
			while(aux < buffer + len)
			{
				switch (aux[0]) {
				case 'A':
					//Audio
					receiveAudio(aux);
					aux += AudioPacket::SIZE;
					break;
				case 'C':
					connected_ = true;
					connectToServer();
					aux += 1;
					break;
				case 'P':
					//Player ids info
					receivePlayerInfo(aux);
					aux += PlayerInfoPacket::SIZE;
					break;
				case 'S':
					//Sprite
					receiveSprite(aux);
					aux += SpritePacket::SIZE;
					break;
				case 'L':
					// partida llena (no hacemos más gestión). Lo ideal sería volver al menú o a la pantalla de inicio
					throw std::runtime_error("Partida llena. No se puede jugar.");
					break;
				}
			}
		}
		else std::cout << "NO RECIBO AAAA\n";
	}
}

void ClientState::connectToServer()
{
	PlayerInfoPacket pInfo;
	pInfo.numberOfPlayers = SDL_Game::instance()->getStateMachine()->getMatchInfo()->getNumberOfPlayers();
	pInfo.player1Info = (char)(*playerInfoVector_)[0]->playerSkin;
	pInfo.player2Info = (char)(*playerInfoVector_)[1]->playerSkin;
	pInfo.player3Info = (char)(*playerInfoVector_)[2]->playerSkin;
	
    if(socket.send(pInfo, socket) == -1)
        std::cout << "no se pudo enviar\n";
}

void ClientState::render()
{
	int currentTime = SDL_Game::instance()->getTime();
	double framesBetween = (currentTime - lastUpdateInstant) / MS_PER_FRAME;

	spriteMutex.lock();
	for(SpritePacket sp : spritesToRender_)
	{
		int posX = sp.posX + sp.velX * framesBetween;
		int posY = sp.posY + sp.velY * framesBetween;
		int angle = sp.rotationDegrees + sp.angVel * framesBetween;
		
		SDL_Game::instance()->getTexturesMngr()->getTexture(sp.textureId)->
			render({ posX, posY, sp.width, sp.height }, angle, (us)sp.frameNumberX, (us)sp.frameNumberY, (SDL_RendererFlip)sp.flip);
	}
	spriteMutex.unlock();
}

void ClientState::receiveSprite(char* aux)
{
	SpritePacket spP;
	spP.from_bin(aux);
	spriteMutex.lock();
	spritesToRender_.push_back(spP);
	spriteMutex.unlock();
}


void ClientState::handleInput()
{
	SDL_Game::instance()->getInputHandler()->update();
	int schar = sizeof(char);
	int sbool = sizeof(bool);
	int sfloat = sizeof(float);
	for (MatchInfo::PlayerInfo* pInfo : *playerInfoVector_)
	{
		//send input
		InputPacket pInputPacket = pInfo->inputBinder->getInputPacket();
		pInputPacket.instant = SDL_Game::instance()->getTime();
		socket.send(pInputPacket, socket);
	}
}

void ClientState::receiveAudio(char* aux)
{
	AudioPacket aP;
	aP.from_bin(aux);
	if(aP.isMusic) 
		SDL_Game::instance()->getAudioMngr()->playMusic(aP.soundId, aP.nLoops);
	else
		SDL_Game::instance()->getAudioMngr()->playChannel(aP.soundId, aP.nLoops);
}

void ClientState::receivePlayerInfo(char* aux)
{
	PlayerInfoPacket pInfoP;
	pInfoP.from_bin(aux);

	(*playerInfoVector_)[0]->playerId = pInfoP.player1Info;
	(*playerInfoVector_)[1]->playerId = pInfoP.player2Info;
	(*playerInfoVector_)[2]->playerId = pInfoP.player3Info;
}