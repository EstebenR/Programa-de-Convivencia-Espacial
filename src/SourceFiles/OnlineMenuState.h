#pragma once
#include "GameState.h"
#include "checkML.h"
#include "Entity.h"
#include "Resources.h"

class InputBinder;
class UIViewer;
class OnlineMenuState :
	public GameState
{
private:
	int pointer_ = 0; //Que opcion esta seleccionada
	bool holdingY_ = false;
	int ownerPlayerID_ = 0;
	InputBinder* ownerPlayerBinder_ = nullptr;

	int tinkyOffset_ = CONST(int, "WINDOW_WIDTH") / 4;

	int xOffset_ = CONST(int, "WINDOW_WIDTH") * 0.5f;
	int yOffset_ = CONST(int, "WINDOW_HEIGHT") / 7;

	int numberOfRounds_ = 3;

	Texture* fondo_ = nullptr;

	Texture* cursorTexture_ = nullptr;

	UIViewer* menuCursor_ = nullptr;
	UIViewer* menuCursor2_ = nullptr;
	std::vector<Entity*> texts_;
	std::vector<Texture*> textures_;
	std::vector<pair<GamemodeID, string>> roundsVector_;

	void updatePointer(int n);
	void createText();

public:
	OnlineMenuState(int playerID = 0) : GameState(), ownerPlayerID_(playerID) {};
	virtual ~OnlineMenuState() {
	};

	virtual void init() override;
	virtual void render() override;
	virtual void onLoaded() override;
	virtual void handleInput() override;
};
