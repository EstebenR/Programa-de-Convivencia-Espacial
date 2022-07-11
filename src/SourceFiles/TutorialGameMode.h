#pragma once
#include "GameMode.h"
#include "Health.h"
#include "HealthViewer.h"
#include "checkML.h"

class TutorialGameMode : public GameMode
{
private:

	//---------------------
	vector<int> playerStocks_; //Remaining stocks of each player
	vector<Entity*> roundResults_; //Used to determine winner and mode end
	vector<Health*> playersHealth_; //Reference to the Health component of each player
	vector<b2Vec2> playersStocksPos_; //Position in which the Stocks will be drawn FOR EACH DIFFERENT PLAYER.
	vector<InputBinder*> inputBinders_;
	int healtWidth_;
	int maxStocks_;

	//tutorial
	vector<Entity*> weapons_;
	vector<Entity*> carnivorousPlants_;
	int tutorialPointer_ = 0;
	vector<bool> tutorials_[Resources::TutorialEnd - Resources::MoveTutorial]; //move, hold, grab, action, throw, death
	vector<Entity*> numberTexts_;
	vector<Entity*> tutorialTexts_;
	Entity* completed_;
	Entity* slash_;
	Entity* numPlayers_;
	double xOffset_ = CONST(int, "WINDOW_WIDTH") * 0.13333333333333;
	double yOffset_ = CONST(int, "WINDOW_HEIGHT") * 2.5;
	double xOffsetProgressText_ = CONST(int, "WINDOW_WIDTH") * 0.2857142857 + CONST(int, "WINDOW_WIDTH") * 0.0833333333;
	double xOffsetProgressCount_ = CONST(int, "WINDOW_WIDTH") * 2.45 + CONST(int, "WINDOW_WIDTH") * 0.125;
	double xOffsetProgressSlash_ = CONST(int, "WINDOW_WIDTH") * 2.35 + CONST(int, "WINDOW_WIDTH") * 0.125;
	double xOffsetProgressMax_ = CONST(int, "WINDOW_WIDTH") * 2.25 + CONST(int, "WINDOW_WIDTH") * 0.125;
	double yOffsetProgress_ = CONST(int, "WINDOW_HEIGHT") * 0.666666666666;
	int stockWidth_ = CONST(int, "STOCK_WIDTH");
	int stockHeight_ = CONST(int, "STOCK_HEIGHT");
	int stockOffset_ = CONST(int, "STOCK_OFFSET");

	double scale_ = 0.75;
	int previousProgress_ = 0;
	bool startedTutorial_ = false;
public:
	TutorialGameMode(MatchInfo* mInfo, int stocks = 5);
	~TutorialGameMode();
	virtual void init(PlayState* game);
	virtual void render();
	virtual void update();
	virtual void activateControl();
	virtual bool onPlayerDead(int id); //Returns false when players runs out of stocks.
};