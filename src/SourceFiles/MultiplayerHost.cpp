#include "MultiplayerHost.h"
#include "SDL_Game.h"
#include "MatchInfo.h"
#include <vector>
#include <sstream>
#include <iostream>

MultiplayerHost::MultiplayerHost() : MultiplayerHost(2000)
{
}

MultiplayerHost::MultiplayerHost(int port)
{
	hostIPAddress_ = getHostIpAddress();
	std::cout << "IP del host es: " << hostIPAddress_ << std::endl;

	socket = new Socket(hostIPAddress_.c_str(), std::to_string(port).c_str());
	init();
}

MultiplayerHost::MultiplayerHost(const char *addr, const char *port)
{
	socket = new Socket(addr, port);
	init();
}

void MultiplayerHost::init()
{
	if (!socket->correct())
		throw std::runtime_error("Host socket initialization failed");

	for (int i = 0; i < 3; i++)
		clients_[i] = nullptr;

	buffer = (char *)malloc(Socket::MAX_MESSAGE_SIZE);
	memset(message, 0, Socket::MAX_MESSAGE_SIZE);
	messagePtr = message + sizeof(unsigned int) + sizeof(uint16_t);

	rcvThread = new std::thread(&MultiplayerHost::rcv, this);
}

MultiplayerHost::~MultiplayerHost()
{
	delete socket;
	socket = nullptr;
	delete rcvThread;
	rcvThread = nullptr;
	delete buffer;
	buffer = nullptr;
}

// este método hace uso de SDL_Net porque poco o nada tiene que ver con el resto del proyecto. En la práctica, no se usa.
std::string MultiplayerHost::getHostIpAddress()
{
	if (SDLNet_Init() < 0)
	{
		throw std::runtime_error("Couldn't start SDLNet");
	}

	//Thanks Samir for all you've done for us
	IPaddress ip;
	if (SDLNet_ResolveHost(&ip, "ipinfo.io", 80) < 0)
		throw std::runtime_error("Couldn't resolve host");
	TCPsocket conn = SDLNet_TCP_Open(&ip);
	if (!conn)
		throw std::runtime_error("Couldn't open TCP conn");

	// envia peticion para conseguir la ip
	const char *getter = "GET /ip HTTP/1.0\nHost: ipinfo.io\n\n";
	int size = strlen(getter);
	int result = SDLNet_TCP_Send(conn, getter, size);
	if (result != size)
		throw;

	// get the response, we assume it is at most bufferSize chars
	const int bufferSize = 255;
	char auxBuffer[bufferSize + 1];
	int read = 0;
	memset(auxBuffer, '\0', bufferSize + 1);
	while (read < bufferSize)
	{
		result = SDLNet_TCP_Recv(conn, auxBuffer + read, bufferSize);
		if (result < 0)
		{
			throw;
		}
		else if (result == 0)
		{
			break;
		}
		else
		{
			read = read + result;
		}
	}

	// The IP is the last token of the response
	std::vector<std::string> tokens;
	std::stringstream iss(auxBuffer);
	for (std::string s; iss >> s;)
	{
		tokens.push_back(s);
	}

	SDLNet_TCP_Close(conn);

	SDLNet_Quit();

	return tokens.back();
}

void MultiplayerHost::rcv()
{
	while (!SDL_Game::instance()->isExit())
	{
		std::cout << "AAAA\n";

		Socket *client = nullptr;

		if (socket->recv(buffer, client) != -1)
		{
			// hay actividad
			checkJoin(client);
			handlePlayerActivity(client);
		}
	}
}

void MultiplayerHost::checkJoin(Socket *client)
{
	// comprobar si ya está conectado
	bool alreadyConn = false;
	int i = 0;
	while (i < 3 && (clients_[i] == nullptr || *client != *(clients_[i].get())))
		i++;

	if (i < 3) // aún no está conectado
	{
		int clientPlace = 0;
		while (clientPlace < 3 && clients_[clientPlace] != nullptr)
			clientPlace++;

		// cabe el cliente
		if (clientPlace < 3)
		{
			//std::cout << "Jugador conectado, id asignada: " << clientPlace << std::endl;
			clientsMutex.lock();
			clients_[clientPlace].reset(client);
			clientsMutex.unlock();

			char myStr = 'C';
			socket->send(&myStr, *clients_[clientPlace].get(), 1);
		}
		// tiramos la conexión, no cabe
		else
		{
			char myStr = 'L';
			socket->send(&myStr, *clients_[clientPlace].get(), 1);
			client = nullptr;
		}
	}
}

void MultiplayerHost::handlePlayerActivity(Socket *client)
{
	//Revisar actividad de los demas
	for (int i = 0; i < 3; i++)
	{
		if (clients_[i] != nullptr)
		{
			memset(buffer, 0, MAX_PACKET_SIZE);
			Socket *s = 0;

			if (socket->recv(buffer, s, 1) == -1)
			{
				//Usuario ha desconectado o perdido conexion
				clientsMutex.lock();
				clients_[i] = nullptr;
				clientsMutex.unlock();
			}
			else
			{
				//Tratamiento de mensajes de los jugadores
				switch (buffer[0])
				{
				case 'P':
					handlePlayerJoin(i);
					break;
				case 'I':
					//Nos llega input del jugador
					handlePlayerInput(i);
					break;
				}
			}
		}
	}
}

void MultiplayerHost::handlePlayerJoin(int clientNumber)
{
	clientsMutex.lock();
	Socket *clientSocket = clients_[clientNumber].get();
	clientsMutex.unlock();

	PlayerInfoPacket piPacket;

	if (socket->recv(piPacket, clientSocket) == -1)
	{
		std::cout << "Couldn't recv in handlePlayerJoin";
		clientsMutex.lock();
		clients_[clientNumber] = nullptr;
		clientSocket = nullptr;
		clientsMutex.unlock();
		return;
	}

	//Revisamos si caben el numero que quieren entrar
	int nStartingPlayers = SDL_Game::instance()->getStateMachine()->getMatchInfo()->getNumberOfPlayers();

	if (piPacket.numberOfPlayers + nStartingPlayers < 5)
	{
		//Aceptamos
		std::vector<MatchInfo::PlayerInfo *> *playersVector = SDL_Game::instance()->getStateMachine()->getMatchInfo()->getPlayersInfo();
		for (int i = 0; i < piPacket.numberOfPlayers; i++)
		{
			playersVector->push_back(new MatchInfo::PlayerInfo(SDL_Game::instance()->getStateMachine()->getMatchInfo()->getNumberOfPlayers(), new ClientBinder(), buffer[i + 1]));
			SDL_Game::instance()->getStateMachine()->getMatchInfo()->updateNumberOfPlayers();
		}

		//Enviamos de vuelta al cliente la informacion de los jugadores
		piPacket.updatePlayersId(nStartingPlayers);
		socket->send(piPacket, *clientSocket);
	}
	else
	{ //Ya hay demasiados jugadores, avisamos que estamos llenos y tiramos la conexion
		char myStr = 'L';
		socket->send(&myStr, *clientSocket, 1);
		clientsMutex.lock();
		clients_[clientNumber] = nullptr;
		clientsMutex.unlock();
	}
}

void MultiplayerHost::handlePlayerInput(int clientNumber)
{
	//receivedBytes_ = SDLNet_TCP_Recv(clients_[clientNumber], buffer, InputPacket::SIZE);

	InputPacket inputPacket;
	Socket *cSocket = clients_[clientNumber].get();
	if (socket->recv(inputPacket, cSocket) == -1)
		return;

	// lo metemos para procesar si es más reciente
	if (inputPacket.instant > inputPackets[inputPacket.playerId].first.instant)
	{
		inputMutex.lock();
		inputPackets[inputPacket.playerId] = std::pair<InputPacket, bool>(inputPacket, true);
		inputMutex.unlock();
	}
}

void MultiplayerHost::addFrameId()
{
	memcpy(messagePtr, &frameId, sizeof(unsigned int));
	messagePtr += sizeof(unsigned int);
}

void MultiplayerHost::addTexture(SpritePacket &sPacket)
{
	sPacket.to_bin();
	packetMutex.lock();
	memcpy(messagePtr, sPacket.data(), sPacket.size());
	messagePtr += sPacket.size();
	packetMutex.unlock();
}

void MultiplayerHost::addAudio(AudioPacket &aPacket)
{
	aPacket.to_bin();
	packetMutex.lock();
	memcpy(messagePtr, aPacket.data(), aPacket.size());
	messagePtr += aPacket.size();
	packetMutex.unlock();
}

void MultiplayerHost::processInput()
{
	std::vector<MatchInfo::PlayerInfo *> *playersVector = SDL_Game::instance()->getStateMachine()->getMatchInfo()->getPlayersInfo();

	for (int i = 0; i < 3; i++)
	{
		inputMutex.lock();
		// es un paquete sin procesar
		if (inputPackets[i].second)
		{
			InputPacket *inputPacket = &inputPackets[i].first;
			(*playersVector)[inputPacket->playerId]->inputBinder->syncInput(*inputPacket);
			inputPackets[i].second = false;
		}
		inputMutex.unlock();
	}
}

// colocar el indicador de tamaño en el segundo espacio de la cabecera, despues del indicador de frame
int MultiplayerHost::finishSending()
{
	uint16_t size = (messagePtr - &message[0]) - (sizeof(unsigned int) + sizeof(uint16_t));
	messagePtr = message;
	addFrameId();
	memcpy(messagePtr, &size, sizeof(uint16_t));
	messagePtr += sizeof(uint16_t);

	//uint8_t securitySecuence = 0xFF;
	//memcpy(messagePtr, &securitySecuence, sizeof(uint8_t));

	++frameId;

	return size + sizeof(unsigned int) + sizeof(uint16_t); // + sizeof(uint8_t);
}

void MultiplayerHost::send()
{
	packetMutex.lock();
	int len = finishSending();

	for (int i = 0; i < 3; i++)
	{
		clientsMutex.lock();
		if (clients_[i] != nullptr)
		{
			clients_[i].get()->send(message, *clients_[i].get(), len);
		}
		clientsMutex.unlock();
	}
	//printf("message: %.*s\n", len, message);
	//std::cout << message << "\0\n";
	memset(message, 0, Socket::MAX_MESSAGE_SIZE);
	messagePtr = message + sizeof(unsigned int) + sizeof(uint16_t);
	packetMutex.unlock();
}