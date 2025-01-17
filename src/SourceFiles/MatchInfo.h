#pragma once
#include "InputBinder.h"
#include "Constants.h"
#include "checkML.h"

struct PlayerLobbyInfo;

class MatchInfo
{
public:
	struct PlayerInfo {
		PlayerInfo(const PlayerLobbyInfo& info);
		PlayerInfo(size_t pId, InputBinder* ib, size_t pSkin);
		~PlayerInfo() { };

		size_t playerId = 0;
		InputBinder* inputBinder = nullptr;
		size_t playerSkin = 0;
		vector<size_t> matchesWon;
		size_t totalPoints = 0;
	};
private:
	vector<PlayerInfo*> players_;
	size_t numberOfPlayers_;

	vector<std::pair<GamemodeID, string>>* rounds_ = nullptr;
	size_t numberOfRounds_;
	size_t currentRoundNumber_ = 0;
public:

	//MatchInfo(size_t numberOfPlayers) { players_.reserve(numberOfPlayers); };
	MatchInfo(const vector<PlayerLobbyInfo>& playerLobbyInfoVector);
	~MatchInfo();

	void setRounds(vector<pair<GamemodeID, string>>* roundsVector) {
		rounds_ = roundsVector; currentRoundNumber_ = 0;
		numberOfRounds_ = (*rounds_).size();
		for (PlayerInfo* p : players_) {
			p->totalPoints = 0;
			for_each(p->matchesWon.begin(), p->matchesWon.end(), [](size_t& n) {n = 0; });
		}
	};

	vector<PlayerInfo*>* getPlayersInfo() { return &players_; };

	pair<GamemodeID, string> getCurrentRound();
	size_t getCurrentRoundNumber() { return currentRoundNumber_; };
	size_t getNumberOfRounds() { return numberOfRounds_; };
	size_t getNumberOfPlayers() { return numberOfPlayers_; };
	void updateNumberOfPlayers() { numberOfPlayers_ = players_.size(); };
	void AddVictory(size_t toPlayer, GamemodeID gameModeWon);
};
