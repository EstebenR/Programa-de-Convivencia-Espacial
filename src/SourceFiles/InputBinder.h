#pragma once
#include <utility>
#include "InputHandler.h"
#include "SDL_Game.h"
#include "Collider.h"
#include "box2d.h"
#include "Constants.h"

class b2Vec2;
enum ActionKey { Grab = 0, Throw, Pick, Attack, Impulse };
struct KeyCursor {
	SDL_Keycode Up = SDLK_w;
	SDL_Keycode Left = SDLK_a;
	SDL_Keycode Down = SDLK_s;
	SDL_Keycode Right = SDLK_d;
	KeyCursor() {}
	KeyCursor(int player) {
		switch (player)
		{
		case 1: {
			Up = SDLK_w;
			Left = SDLK_a;
			Down = SDLK_s;
			Right = SDLK_d;
		}
			  break;
		case 2: {
			Up = SDLK_i;
			Left = SDLK_j;
			Down = SDLK_k;
			Right = SDLK_l;
		}
			  break;
		default:
			break;
		}
	}
	KeyCursor(SDL_Keycode u, SDL_Keycode l, SDL_Keycode d, SDL_Keycode r) {
		Up = u; Left = l, Down = d; Right = r;
	}
};
struct KeyboardMapping {
	KeyCursor cursor; //Teclas de movimiento para PureKeyboardBinder
	//Varias teclas
	SDL_Keycode grab,
		pickWeapon,
		pickWeapon_secondary,
		throwWeapon,
		throwWeapon_secondary,
		attack,
		attack_secondary,
		impulse,
		impulse_secondary,
		pause,
		forward,
		back;

	KeyboardMapping(int player = 1) {
		switch (player)
		{
		case 1: {
			cursor = KeyCursor(1);
			grab = SDLK_LSHIFT,
				pickWeapon = SDLK_q,
				pickWeapon_secondary = SDLK_e,
				throwWeapon = SDLK_q,
				throwWeapon_secondary = SDLK_e,
				attack = SDLK_z,
				attack_secondary = SDLK_r,
				impulse = SDLK_TAB,
				impulse_secondary = SDLK_f,
				pause = SDLK_ESCAPE,
				forward = SDLK_e,
				back = SDLK_q;
		}
			  break;
		case 2: {
			cursor = KeyCursor(2);
			grab = SDLK_RSHIFT,
				pickWeapon = SDLK_o,
				pickWeapon_secondary = SDLK_COMMA,
				throwWeapon = SDLK_o,
				throwWeapon_secondary = SDLK_COMMA,
				attack = SDLK_p,
				attack_secondary = SDLK_PERIOD,
				impulse = SDLK_RETURN,
				impulse_secondary = SDLK_RETURN,
				pause = SDLK_BACKSPACE,
				forward = SDLK_o,
				back = SDLK_u;
		}
			  break;
		default:
			break;
		}
	}
};
enum Dir { Up = 0, Down, Left, Right };

//Abstracta pura
class InputBinder
{
protected:
	InputHandler* ih = nullptr;
	b2Vec2 lastDir = b2Vec2(0, 0);
private:
public:
	InputBinder() { ih = SDL_Game::instance()->getInputHandler(); }
	virtual ~InputBinder() {}
	virtual bool holdGrab() = 0;
	virtual bool pressThrow() = 0;
	virtual bool pressPick() = 0;
	virtual bool releaseGrab() = 0;
	virtual b2Vec2 getAimDir() = 0;
	virtual bool pressImpulse() = 0;
	virtual bool holdImpulse() = 0;
	virtual bool releaseImpulse() = 0;
	virtual bool pressAttack() = 0;
	virtual void setPlayerCol(Collider* col) {};
	virtual bool menuMove(Dir d) = 0;
	virtual bool menuForward() = 0;
	virtual bool menuBack() = 0;
	virtual bool pressPause() = 0;
	virtual bool secretWinButton() { return false; }
	virtual InputPacket getInputPacket();
	virtual void syncInput(InputPacket p) {};
};

//Abstracta pura para modos con teclado
//para que dejeis
class KeyboardBinder : public InputBinder {
protected:
	KeyboardMapping map_;
public:
	KeyboardBinder(KeyboardMapping m) : InputBinder(), map_(m) {}
	KeyboardBinder(int defaultMap) : InputBinder(), map_(defaultMap) {}
	bool grabbed = false;
	virtual bool holdGrab() {
		return (!grabbed && ih->isKeyDown(map_.grab));
	}
	virtual bool releaseGrab() {
		return(grabbed && ih->isKeyJustDown(map_.grab));
	}
	virtual bool pressPick() {
		return (ih->isKeyJustDown(map_.pickWeapon) || ih->isKeyJustDown(map_.pickWeapon_secondary));
	}
	virtual bool pressThrow() {
		return (ih->isKeyJustDown(map_.throwWeapon) || ih->isKeyJustDown(map_.throwWeapon_secondary));
	}
	virtual bool menuMove(Dir d) {
		switch (d) {
		case Dir::Up: { return ih->isKeyDown(map_.cursor.Up); }			break;
		case Dir::Down: { return ih->isKeyDown(map_.cursor.Down); }		break;
		case Dir::Left: { return ih->isKeyDown(map_.cursor.Left); }		break;
		case Dir::Right: { return ih->isKeyDown(map_.cursor.Right); }	break;
		default: { return false; }											break;
		}
	}
	virtual bool menuForward() {
		return ih->isKeyJustDown(map_.forward);
	}
	virtual bool menuBack() {
		return ih->isKeyJustDown(map_.back);
	}
	virtual bool pressPause() { return ih->isKeyJustDown(map_.pause); }
	//como sigamos con la pelea juro que me como a alguien
	virtual b2Vec2 getAimDir() = 0;
};

//de llorar todos
class PureKeyboardBinder : public KeyboardBinder {
	//Team MMur, PJ, posiblemente Satan
public:
	PureKeyboardBinder(KeyboardMapping m) : KeyboardBinder(m) {}
	PureKeyboardBinder(int defaultMap) : KeyboardBinder(defaultMap) {}
	virtual b2Vec2 getAimDir() {
		b2Vec2 dir = b2Vec2(0, 0);
		if (ih->isKeyDown(map_.cursor.Left))
			dir.x = -1;
		else if (ih->isKeyDown(map_.cursor.Right))
			dir.x = 1;
		if (ih->isKeyDown(map_.cursor.Up))
			dir.y = -1;
		if (ih->isKeyDown(map_.cursor.Down))
			dir.y = 1;
		dir.Normalize();
		if (dir.y != 0 || dir.x != 0)
			lastDir = dir;
		return lastDir;
	}
	virtual bool pressImpulse() {
		return (ih->isKeyJustDown(map_.impulse) || ih->isKeyJustDown(map_.impulse_secondary));
	}
	virtual bool holdImpulse() {
		return (ih->isKeyDown(map_.impulse) || ih->isKeyDown(map_.impulse_secondary));
	}
	virtual bool releaseImpulse() {
		return (ih->isKeyJustUp(map_.impulse) || ih->isKeyJustUp(map_.impulse_secondary));
	}
	virtual bool pressAttack() {
		return (ih->isKeyJustDown(map_.attack) || ih->isKeyJustDown(map_.attack_secondary));
	}
};

//de una puta vez
class MouseKeyboardBinder : public KeyboardBinder {
	//Team Adri, Esteban, Andres, Jorge
	Collider* playerCol_ = nullptr;
public:
	MouseKeyboardBinder(Collider* c, KeyboardMapping m) : KeyboardBinder(m), playerCol_(c) {}
	MouseKeyboardBinder(Collider* c, int defaultMap) : KeyboardBinder(defaultMap), playerCol_(c) {}
	virtual b2Vec2 getAimDir() {
		//devolvemos un vector unitario que apunte del jugador al raton
		if (playerCol_ != nullptr) {
			SDL_Rect playerDrawPos = playerCol_->getRectRender();
			b2Vec2 playerPos = b2Vec2(playerDrawPos.x + playerDrawPos.w * 0.5f, playerDrawPos.y + playerDrawPos.h * 0.5f);
			b2Vec2 dir = ih->getMousePos() - playerPos;
			dir.Normalize();
			if (dir.y != 0 || dir.x != 0)
				lastDir = dir;
			return lastDir;
		}
		else {
			//cout << "no hay collider\n";
			return b2Vec2();
		}
	}
	virtual bool pressImpulse() {
		return ih->isMouseButtonJustDown(InputHandler::MOUSEBUTTON::RIGHT);
	}
	virtual bool holdImpulse() {
		return ih->isMouseButtonDown(InputHandler::MOUSEBUTTON::RIGHT);
	}
	virtual bool releaseImpulse() {
		return ih->isMouseButtonJustUp(InputHandler::MOUSEBUTTON::RIGHT);
	}
	virtual bool pressAttack() {
		return ih->isMouseButtonJustDown(InputHandler::MOUSEBUTTON::LEFT);
	}
	virtual void setPlayerCol(Collider* col) override { playerCol_ = col; };
};

//La unica opcion correcta
class ControllerBinder : public InputBinder {
protected:
	int id_ = -1;
public:
	ControllerBinder(int id) : InputBinder(), id_(id) {}
	virtual ~ControllerBinder() {}
	virtual bool holdGrab() {
		return ih->getTrigger(id_, InputHandler::GAMEPADTRIGGER::LEFTTRIGGER);
	}
	virtual bool releaseGrab() {
		return !ih->getTrigger(id_, InputHandler::GAMEPADTRIGGER::LEFTTRIGGER);
	}
	virtual bool pressPick() {
		return ih->isButtonJustDown(id_, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_Y);
	}
	virtual bool pressThrow() {
		return ih->isButtonJustDown(id_, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_Y);
	}
	virtual bool pressImpulse() {
		return ih->isButtonJustDown(id_, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A);
	}
	virtual bool holdImpulse() {
		return ih->isButtonDown(id_, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A);
	}
	virtual bool releaseImpulse() {
		return ih->isButtonJustUp(id_, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A);
	}
	virtual bool pressAttack() {
		return ih->isButtonJustDown(id_, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_X);
	}
	virtual b2Vec2 getAimDir() {
		return ih->getLastStickDir(id_, InputHandler::GAMEPADSTICK::LEFTSTICK);
	}
	virtual bool menuMove(Dir d) {
		switch (d) {
		case Dir::Down: { return ih->getStickDir(id_, InputHandler::GAMEPADSTICK::LEFTSTICK).y > 0.9; }		break;
		case Dir::Up: { return ih->getStickDir(id_, InputHandler::GAMEPADSTICK::LEFTSTICK).y < -0.9; }	break;
		case Dir::Left: { return ih->getStickDir(id_, InputHandler::GAMEPADSTICK::LEFTSTICK).x < -0.9; }	break;
		case Dir::Right: { return ih->getStickDir(id_, InputHandler::GAMEPADSTICK::LEFTSTICK).x > 0.9; }	break;
		default: { return false; }																			break;
		}
	}
	virtual bool menuForward() {
		return ih->isButtonJustDown(id_, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A);
	}
	virtual bool menuBack() {
		return ih->isButtonJustDown(id_, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_B);
	}
	virtual bool pressPause() {
		return ih->isButtonJustDown(id_, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_START)
			|| ih->isButtonJustDown(id_, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_GUIDE);
	}
	virtual bool secretWinButton() { return ih->isButtonJustDown(id_, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_LEFTSHOULDER); }
};

class ClientBinder :public InputBinder {
protected:
	InputPacket state;
public:
	ClientBinder() :InputBinder() {};
	virtual void syncInput(InputPacket p) { state = p; }

	virtual bool holdGrab() { return state.holdGrab; }
	virtual bool releaseGrab() { return state.releaseGrab; }
	virtual bool pressThrow() { return state.pressThrow; }
	virtual bool pressPick() { return state.pressPick; }
	virtual b2Vec2 getAimDir() { return b2Vec2(state.aimDirX, state.aimDirY); }
	virtual bool pressImpulse() { return state.pressImpulse; }
	virtual bool holdImpulse() { return state.holdImpulse; }
	virtual bool releaseImpulse() { return state.releaseImpulse; }
	virtual bool pressAttack() { return state.pressAttack; }
	virtual bool menuMove(Dir d) { return (d & state.menuMove) != 0; }
	virtual bool menuForward() { return state.menuForward; }
	virtual bool menuBack() { return state.menuBack; }
	virtual bool pressPause() { return state.pressPause; }
};

//pero seguid con los meme xfa