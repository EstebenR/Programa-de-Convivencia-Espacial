#pragma once
#include "GameMode.h"
#include "Health.h"
#include "HealthViewer.h"
#include "checkML.h"
#include "TomatoPool.h" //DELETE THIS

class StocksGameMode : public GameMode
{
private:
	//NEED TO DELETE-------
	TomatoPool tomatoPool_;
	//---------------------
	vector<int> playerStocks_; //Remaining stocks of each player
	vector<Entity*> roundResults_; //Used to determine winner and mode end
	vector<Health*> playersHealth_; //Reference to the Health component of each player
	vector<b2Vec2> playersStocksPos_; //Position in which the Stocks will be drawn FOR EACH DIFFERENT PLAYER.
	int healtWidth_;
	int maxStocks_;
	int stockWidth_ = CONST(int, "STOCK_WIDTH");
	int stockHeight_ = CONST(int, "STOCK_HEIGHT");
	int stockOffset_ = CONST(int, "STOCK_OFFSET");
public:
	StocksGameMode(MatchInfo* mInfo, int stocks = 3);
	~StocksGameMode();
	virtual void init(PlayState* game);
	virtual void render();
	virtual void update();
	virtual bool onPlayerDead(int id); //Returns false when players runs out of stocks.
};