#include "SDL_Game.h"
#include "checkML.h"
#include "json.hpp"
#include "SDL_net.h"
#include <fstream>
#include <iomanip>

using json = nlohmann::json;

int main(int argc, char* argv[])
{
	//Colocar antes del constructor de SDL_Game
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	try {
		if(argc == 1)	SDL_Game::instance()->start();
		else 			SDL_Game::instance()->start(argv[1], argv[2]);
	}
	catch(std::runtime_error& e) {
		std::cout << e.what() << "\n";
	}
	catch(std::exception& e) {
		std::cout << e.what() << "\n";
	}
	catch(std::string& s){
		std::cout << s << "\n";
	}
	catch(const char* s){
		std::cout << s << "\n";
	}
	catch(...) {
		std::cout << "unknown error\n";
	}

	//descomentar para depurar
	//cuando todo era simple
	//while (true) {};
	return 0;
}