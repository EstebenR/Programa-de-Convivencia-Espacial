#pragma once
#include "GameMode.h"
#include "GameMode.h"
#include "Health.h"
#include "HealthViewer.h"
#include "box2d.h"
#include "Hands.h"
#include "PlayerData.h"
#include "ObjectFactory.h"
#include "PlayState.h"
#include "GameState.h"
#include "Weapon.h"
#include "checkML.h"

class ControllerGameMode : public GameMode
{
private:
	vector<double> controllerTimes_;
	Weapon* controller_ = nullptr;
	int halfWidth_ = CONST(int, "WINDOW_WIDTH") * 0.5;
	int halfHeight_ = CONST(int, "WINDOW_HEIGHT") * 0.5;
	double msPerFrame_ = CONST(double, "MS_PER_FRAME");
	double timeToWin_ = CONST(double, "TIME_TO_WIN");
public:
	ControllerGameMode(MatchInfo* mInfo) : GameMode(mInfo, GamemodeID::Controller) {};
	virtual ~ControllerGameMode();
	virtual void init(PlayState* game);
	virtual void render();
	virtual void update();
	vector<double> getTimes() { return controllerTimes_; }
};
