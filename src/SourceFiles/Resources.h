#pragma once

#include <SDL.h>
#include <string>
#include <vector>
#include "checkML.h"
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

static string resourcesPath = "../../assets/game/";

class Resources {
public:

	enum TextureId : unsigned char {
		// images

		// room backgrounds
		MainMenuRoom,
		BoilerRoom,
		GymRoom,
		LivingRoom,
		RocketRoom,
		TutorialRoom,
		GardenRoom,
		MenuRoom,
		CreditsRoom,
		SpaceBackground,

		// UI
		ActiveHealth,
		DisableHealth,

		Slider,
		SliderControl,

		CanvasTimerBackground,
		CoinUI,
		SkullUI,
		SuddenDeathCapitalismModeText,
		SuddenDeathTimeModeText,

		ProgressBar,


		CursorUiSelectMap,
		CapitalismIcon,
		ControllerModeIcon,
		StocksIcon,
		WiFightIcon,
		TimedIcon,

		Ready,
		PressReady,


		//Lobby
		KeyboardIcon,
		MouseIcon,
		ControllerIcon,
		PlayerPlaceholder,

		//midGameState
		Rocket,
		SpaceStation,
		Token,
		RocketSpriteSheet,

		// Weapons and map items

		// Players
		PlayerAnimSheet,
		PlayerAnimSheetLobby,
		PlayerHeads,


		// Map elements
		IncreasingFreqButton,
		DecreasingFreqButton,
		TSet_Kenney1bit,
		TSet_Dungeon,
		Slipper,
		Extinguisher,
		Dumbbell,
		Ball,
		Stapler,
		Stone,
		Body,
		Hands,
		SpaceSuit,
		ImpulseArrow,
		ImpulseBackground,
		Fireball,
		Boiler,
		PadSpriteSheet,
		Treadmill,
		TreadmillSpriteSheet,
		PipeHor,
		PipeVer,
		TreeTomato,
		CarnivorousPlant,
		Tomato,
		TomatoRing,
		TreadmillPanel,
		Banana,
		BananaSkin,
		BananaNonSkin,
		Staple,
		Confetti,
		ConfettiParticles,
		Lamp,
		Table,
		Sofa,
		TomatoTree,
		BananaTree,
		DeadBody,
		TriggerButtonPlay,
		TriggerButtonExit,
		TriggerButtonCredits,
		TriggerButtonOptions,

		// Gamemode specific elements
		Coin,
		Router,
		RoombaSpriteSheet,
		Remote,

		// miscel�nea
		Debug,
		Negro,
		Tinky,
		Transparent,

		// text
		PauseText,
		ResumeText,
		MusicText,
		EffectsText,
		MainMenuText,

		//online menu
		Local,
		Host,
		Join,
		Tutorial,

		//modes
		Capitalism,
		Controller,
		Stocks,
		WiFight,
		Timed,
		//exit
		Exit,
		//maps
		LivingRoomText,
		BoilerRoomText,
		GymRoomText,

		GardenRoomText,
		//Menu eleccion de mapas
		ChoseMap,
		ChoseMode,
		PressEnter,


		//tutorial
		Completed,
		Slash,
		Zero,
		One,
		Two,
		Three,
		Four,
		Five,
		Six,
		Seven,
		Eight,
		Nine,
		MoveTutorial,
		HoldTutorial,
		GrabTutorial,
		ActionTutorial,
		ThrowTutorial,
		DeathTutorial,
		TutorialEnd,

		//MidGame
		ContinueText,

		//Countdown
		Go,

		// particles
		Smoke,
		WiFiWave,
		Foam,

		// numbers
		NumZero,
		NumOne,
		NumTwo,
		NumThree,
		NumFour,
		NumFive,
		NumSix,
		NumSeven,
		NumEight,
		NumNine,

		//timers
		Colon,

		//badges ordenadas como GameModeId
		badgeCapitalism,
		badgeController,
		badgeStocks,
		badgeWifi,
		badgeTimed,

		medals,

		//winner msgs
		winner1,
		winner2,
		winner3,
		winner4,

		//Credits
		ReturnText,

		//Options menu
		OptionsText,
		GoBackText,
	};

	enum AudioId : unsigned char {
		// music
		MainMenuMusic = TextureId::MainMenuRoom,
		LivingRoomMusic = TextureId::LivingRoom,
		GymMusic = TextureId::GymRoom,
		BoilerRoomMusic = TextureId::BoilerRoom,
		TutorialMusic = TextureId::TutorialRoom,
		GardenMusic = TextureId::GardenRoom,
		EntranceMusic,

		// sound effects
		StartGame,
		ThreeTwoOneSound,
		GoSound,
		PadSound,
		PickSound,
		CoinSound,
		// boiler
		BoilerShootSound,
		BoilerButtonSlowSound,
		BoilerButtonFastSound,

		CarnivorousPlantSound,

		// Weapons
		BananaShootSound,
		ExtinguisherSound,
		TomatoExplosionSound,
		ConfettiSound,

		//Player
		DeathSound,
		RespawnSound,
		EliminatedSound,
		ImpulseFromAirSound,
		ImpulseAttachedSound,

		//menu
		MenuMove,
		MenuForward,
		MenuBackward
	};

	enum FontId : std::size_t {
		NES_Chimera,
		Digits_Font
	};

	struct FontInfo {
		FontId id;
		string fileName;
		int size;
	};

	struct ImageInfo {
		TextureId id;
		string fileName;
		unsigned short nHorizontalFrames = 1;
		unsigned short nVerticalFrames = 1;
	};

	struct TextMsgInfo {
		TextureId id;
		string msg;
		SDL_Color color;
		FontId fontId;
	};

	struct MusicInfo {
		AudioId id;
		string fileName;
	};

	struct SoundInfo {
		AudioId id;
		string fileName;
	};

	static vector<FontInfo> fonts_; // initialized in .cpp
	static vector<ImageInfo> images_; // initialized in .cpp
	static vector<TextMsgInfo> messages_; // initialized in .cpp
	static vector<MusicInfo> musics_; // initialized in .cpp
	static vector<SoundInfo> sounds_; // initialized in .cpp
	static map<std::string, Resources::TextureId> tilesetTag_; // initialized in .cpp
};
