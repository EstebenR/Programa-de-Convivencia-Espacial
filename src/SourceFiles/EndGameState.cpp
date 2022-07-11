#include "EndGameState.h"
#include "SDL_Game.h"
#include "InputHandler.h"
#include "MatchInfo.h"
#include "GameStateMachine.h"
#include "Texture.h"
#include "SDLTexturesManager.h"
#include "Resources.h"
#include "AnimatedViewer.h"
#include "Entity.h"
#include "AnimatedUIViewer.h"
#include <algorithm>
#include <random>


bool comparePoints(MatchInfo::PlayerInfo* a, MatchInfo::PlayerInfo* b) { return a->totalPoints > b->totalPoints; }

EndGameState::~EndGameState()
{
	delete entityManager_;		entityManager_ = nullptr;
}

void EndGameState::init()
{
	backgroundT_ = SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::RocketRoom);
	vector<MatchInfo::PlayerInfo*>* playerInfo_ = SDL_Game::instance()->getStateMachine()->getMatchInfo()->getPlayersInfo();
	for (auto& player : *playerInfo_) {
		sortedPlayerInfo_.push_back(player);
	}
	sort(sortedPlayerInfo_.begin(), sortedPlayerInfo_.end(), comparePoints);

	int auxX = CONST(int, "LEADERBOARD_ANCHOR_X");
	int auxY = CONST(int, "LEADERBOARD_ANCHOR_Y");
	int iconOffset = CONST(int, "LEADERBOARD_ICON_OFFSET");
	int iconMargin = CONST(int, "LEADERBOARD_ICON_MARGIN");
	int zeroTex = Resources::NumZero;
	b2Vec2 basicMedalWH(CONST(int, "LEADERBOARD_MEDAL_W"), CONST(int, "LEADERBOARD_MEDAL_H"));
	b2Vec2 basicPlayerWH(CONST(int, "LEADERBOARD_PLAYER_W"), CONST(int, "LEADERBOARD_PLAYER_H"));
	b2Vec2 basicBadgeWH(CONST(int, "LEADERBOARD_ICON_SIZE"), CONST(int, "LEADERBOARD_ICON_SIZE"));
	int medalOffset = CONST(int, "LEADERBOARD_MEDAL_OFFSET") + basicPlayerWH.x * 3;

	entityManager_ = new EntityManager();
	int y = auxY;

	for (int i = 0; i < sortedPlayerInfo_.size(); i++) {
		int scale = 3 - i;
		b2Vec2 actPlayerWH(basicPlayerWH.x * scale, basicPlayerWH.y * scale);
		b2Vec2 actMedalWH(basicMedalWH.x * scale, basicMedalWH.y * scale);
		b2Vec2 actBadgeWH(basicBadgeWH.x * scale, basicBadgeWH.y * scale);

		Entity* auxEntity = entityManager_->addEntity();
		int x = auxX - actPlayerWH.x * 0.5f;

		UIViewer* playerV = auxEntity->addComponent<UIViewer>(Resources::PlayerAnimSheet, b2Vec2(x, y), 1);
		playerV->setWHUIElement(actPlayerWH);
		playerV->setFrame(0, sortedPlayerInfo_[i]->playerSkin);

		x = medalOffset - actMedalWH.x * 0.5f;
		y += actMedalWH.y * 0.5f;

		if (i < 3) {//si le toca medallita
			UIViewer* medalV = auxEntity->addComponent<UIViewer>(Resources::medals, b2Vec2(x, y), 1);
			medalV->setFrame(i);
			medalV->setWHUIElement(actMedalWH);
		}
		x += actMedalWH.x;
		y += actBadgeWH.y * 0.5f;

		//lista de chapitas
		vector<int> chapitas;
		for (int j = 0; j < sortedPlayerInfo_[i]->matchesWon.size(); j++) {
			for (int n = 0; n < sortedPlayerInfo_[i]->matchesWon[j]; n++) {
				chapitas.push_back(j);
			}
		}
		auto rng = std::default_random_engine{};
		std::shuffle(chapitas.begin(), chapitas.end(), rng);

		x += iconOffset;
		Resources::TextureId firstBadge = Resources::badgeCapitalism;
		for (auto& chapa : chapitas) {
			UIViewer* badgeV = auxEntity->addComponent<UIViewer>((Resources::TextureId)firstBadge + chapa, b2Vec2(x, y), 1);
			badgeV->setWHUIElement(actBadgeWH);
			x += iconMargin;
			x += actBadgeWH.x;
		}

		y += actPlayerWH.y - actMedalWH.y * 0.5f - actBadgeWH.y * 0.5f;
	}

	b2Vec2 textWH(CONST(int, "LEADERBOARD_TEXT_W"), CONST(int, "LEADERBOARD_TEXT_H"));
	int winTextX = CONST(int, "WINDOW_WIDTH") * 0.5f - textWH.x * 0.5f;
	int winTextY = CONST(int, "LEADERBOARD_TEXT_Y");
	Entity* e = entityManager_->addEntity();
	UIViewer* textV = e->addComponent<UIViewer>(Resources::winner1 + sortedPlayerInfo_[0]->playerId, b2Vec2(winTextX, winTextY), 1);
	textV->setWHUIElement(textWH);
}
void EndGameState::render()
{
	backgroundT_->render(0, 0);
	GameState::render();
}

void EndGameState::update()
{
	if (anyButtonPush) {
		GameStateMachine* gsMachine = SDL_Game::instance()->getStateMachine();
		gsMachine->transitionToState(States::playableMenu);
	}
}

void EndGameState::handleInput()
{
	GameState::handleInput();
	InputHandler* ih = SDL_Game::instance()->getInputHandler();

	if (ih->keyDownEvent() || ih->isButonDownEvent()) {
		anyButtonPush = true;
	}
	else {
		anyButtonPush = false;
	}
}

void EndGameState::onLoaded()
{
	SDL_Game::instance()->getStateMachine()->deleteState(States::midGame);
}