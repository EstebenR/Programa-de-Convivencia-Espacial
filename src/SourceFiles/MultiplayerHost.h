#pragma once

#include "SDL_net.h"
#include <string>
#include "Constants.h"
#include "Socket.h"
#include <mutex>
#include <thread>

class MultiplayerHost
{
protected:

	std::unique_ptr<Socket> clients_[3];	//max 3 jugadores conectados + el host
	Socket* socket = nullptr;

	std::string hostIPAddress_;
	
	int receivedBytes_ = 0;

	char* buffer;

	char message[Socket::MAX_MESSAGE_SIZE];
	char* messagePtr = message;

	unsigned int frameId = 0;
	uint16_t size = 0;

	std::string getHostIpAddress();

	std::thread* rcvThread;
	std::pair<InputPacket, bool> inputPackets[3];		// para procesarlos todos juntos tras recibirlos si es que cambian
	std::mutex inputMutex;
	std::mutex clientsMutex;
	std::mutex packetMutex;

	void checkJoin(Socket* client);
	void handlePlayerActivity(Socket* client);
	void addFrameId();
	void handlePlayerJoin(int clientNumber);
	void handlePlayerInput(int clientNumber);
	int finishSending();

public:
	MultiplayerHost();
	MultiplayerHost(int port);
	MultiplayerHost(const char* addr, const char* port);
	void init();
	~MultiplayerHost();

	void rcv();
	void processInput();
	void addTexture(SpritePacket& sPacket);
	void addAudio(AudioPacket& aPacket);
	void send();
};
