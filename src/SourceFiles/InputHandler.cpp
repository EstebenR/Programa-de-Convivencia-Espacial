#include "InputHandler.h"
#include <fstream>
#include <box2d.h>
#include <iostream>
#include "SDL_Game.h"

using namespace std;
InputHandler::InputHandler() {
	clearState();
	numControllers_ = 0;
	for (int i = 0; i < kbSize; i++) {
		kbState_.push_back(ButtonState::Up);
	}
}

InputHandler::~InputHandler() {
	for (std::pair<Vector2D*, Vector2D*> e : m_joystickValues) {
		delete e.first;
		delete e.second;
	}
	for (std::pair<double*, double*> e : m_triggerValues) {
		delete e.first;
		delete e.second;
	}
}

void InputHandler::update() {
	SDL_Event event;

	clearState();

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			onKeyDown(event);
			break;
		case SDL_KEYUP:
			onKeyUp(event);
			break;
		case SDL_MOUSEMOTION:
			onMouseMotion(event);
			break;
		case SDL_MOUSEBUTTONDOWN:
			onMouseButtonChange(event, true);
			break;
		case SDL_MOUSEBUTTONUP:
			onMouseButtonChange(event, false);
			break;
		case SDL_JOYAXISMOTION:
			onJoyAxisChange(event);
			break;
		case SDL_JOYBUTTONDOWN:
			onJoyButtonChange(event, JustDown);
			break;
		case SDL_JOYBUTTONUP:
			onJoyButtonChange(event, JustUp);
			break;
		case SDL_CONTROLLERDEVICEADDED:
			onControllerAddedEvent(event);
			break;
		case SDL_CONTROLLERDEVICEREMOVED:
			onControllerRemovedEvent(event);
			break;
		case SDL_QUIT:
			SDL_Game::instance()->exitGame();
			break;
		}
	}
}

void InputHandler::initialiseGamepads() {
	if (-1 == SDL_GameControllerAddMappingsFromFile("../config/gamecontrollerdb.txt"))
		cout << "Error al cargar la base de datos" << endl;
	if (SDL_WasInit(SDL_INIT_JOYSTICK) == 0) {
		SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	}
	numControllers_ = SDL_NumJoysticks();
}

void InputHandler::clearJoysticks()
{
	if (m_bJoysticksInitialised)
	{
		for (auto& ctrl : m_gameControllers) {
			SDL_GameControllerClose(ctrl);
		}
	}
}

void InputHandler::clearState() {
	isKeyDownEvent_ = false;
	isKeyUpEvent_ = false;
	isMouseButtonEvent_ = false;
	isMouseMotionEvent_ = false;
	isAxisMovementEvent_ = false;
	isButtonDownEvent_ = false;
	isButtonUpEvent_ = false;

	//Ajustamos los botones y teclas recien pulsados o soltados
	for (int i = 0; i < 3; i++) {
		switch (mbState_[i])
		{
		case JustUp:
			mbState_[i] = Up;
			break;
		case JustDown:
			mbState_[i] = Down;
			break;
		default:
			break;
		}
	}
	while (!justDownKeys.empty()) {
		kbState_[justDownKeys.front()] = ButtonState::Down;
		justDownKeys.pop();
	}
	while (!justUpKeys.empty()) {
		kbState_[justUpKeys.front()] = ButtonState::Up;
		justUpKeys.pop();
	}

	for (int controller = 0; controller < m_gameControllers.size(); controller++) {
		for (int j = 0; j < SDL_CONTROLLER_BUTTON_MAX; j++) {
			switch (m_buttonStates[controller][j])
			{
			case(JustDown):
				m_buttonStates[controller][j] = Down;
				break;
			case(JustUp):
				m_buttonStates[controller][j] = Up;
				break;
			default:
				break;
			}
		}
	}
}

double InputHandler::getStickX(int gameJoy, GAMEPADSTICK stick)
{
	int joy = gameToSystemCtrlId[gameJoy];
	if (joy >= m_gameControllers.size())
		return 0.0;
	if (m_joystickValues.size() > 0)
	{
		if (stick == LEFTSTICK)

		{
			return m_joystickValues[joy].first->getX();
		}
		else if (stick == RIGHTSTICK)
		{
			return m_joystickValues[joy].second->getX();
		}
	}
	return 0;
}

double InputHandler::getStickY(int gameJoy, GAMEPADSTICK stick)
{
	int joy = gameToSystemCtrlId[gameJoy];
	if (joy >= m_gameControllers.size())
		return 0.0;
	if (m_joystickValues.size() > joy)
	{
		if (stick == LEFTSTICK)
		{
			return m_joystickValues[joy].first->getY();
		}
		else if (stick == RIGHTSTICK)
		{
			return m_joystickValues[joy].second->getY();
		}
	}
	return 0;
}

double InputHandler::getTrigger(int gameJoy, GAMEPADTRIGGER trigger) {
	int joy = gameToSystemCtrlId[gameJoy];
	if (m_joystickValues.size() > joy)
	{
		if (trigger == LEFTTRIGGER)
		{
			return *m_triggerValues[joy].first;
		}
		else if (trigger == RIGHTTRIGGER)
		{
			return *m_triggerValues[joy].second;
		}
	}
	return 0;
}

void InputHandler::onJoyAxisChange(SDL_Event& event) {
	isAxisMovementEvent_ = true;
	int whichOne = event.jaxis.which;
	const double normalize = 1.0 / 32768.0;
	double val = event.jaxis.value;
	// left stick move left or right

	Uint8 i = 0;
	bool bindFound = false;
	int bindedAxis;
	//hay que buscar el botón al que se corresponde
	//porque SDL es una librería maravillosa y super intuitiva
	while (!bindFound && i < SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_MAX) {
		SDL_GameControllerButtonBind b = SDL_GameControllerGetBindForAxis(m_gameControllers[whichOne], (SDL_GameControllerAxis)i);
		if (b.value.axis == event.jaxis.axis) {
			bindedAxis = i;
			bindFound = true;
		}
		i++;
	}
	//left stick right or left
	if (bindedAxis == 0)
	{
		if (val > m_joystickDeadZone)
		{
			m_joystickValues[whichOne].first->setX(val * normalize);
			lastLStickValue_[whichOne].x = (float)m_joystickValues[whichOne].first->getX();
		}
		else if (event.jaxis.value < -m_joystickDeadZone)
		{
			m_joystickValues[whichOne].first->setX(val * normalize);
			lastLStickValue_[whichOne].x = (float)m_joystickValues[whichOne].first->getX();
		}
		else
		{
			m_joystickValues[whichOne].first->setX(0);
		}
	}
	// left stick move up or down
	if (bindedAxis == 1)
	{
		if (event.jaxis.value > m_joystickDeadZone)
		{
			m_joystickValues[whichOne].first->setY(val * normalize);
			lastLStickValue_[whichOne].y = (float)m_joystickValues[whichOne].first->getY();
		}
		else if (event.jaxis.value < -m_joystickDeadZone)
		{
			m_joystickValues[whichOne].first->setY(val * normalize);
			lastLStickValue_[whichOne].y = (float)m_joystickValues[whichOne].first->getY();
		}
		else
		{
			m_joystickValues[whichOne].first->setY(0);
		}
	}
	//left trigger move up or down
	if (bindedAxis == 4) {
		if (event.jaxis.value > m_triggerDeadZone)
		{
			*m_triggerValues[whichOne].first = abs(event.jaxis.value);
		}
		else
		{
			*m_triggerValues[whichOne].first = 0;
		}
	}
	// right stick move left or right
	if (bindedAxis == 2)
	{
		if (event.jaxis.value > m_joystickDeadZone)
		{
			m_joystickValues[whichOne].second->setX(val * normalize);
		}
		else if (event.jaxis.value < -m_joystickDeadZone)
		{
			m_joystickValues[whichOne].second->setX(val * normalize);
		}
		else
		{
			m_joystickValues[whichOne].second->setX(0);
		}
	}
	// right stick move up or down
	if (bindedAxis == 3)
	{
		if (event.jaxis.value > m_joystickDeadZone)
		{
			m_joystickValues[whichOne].second->setY(val * normalize);
		}
		else if (event.jaxis.value < -m_joystickDeadZone)
		{
			m_joystickValues[whichOne].second->setY(val * normalize);
		}
		else
		{
			m_joystickValues[whichOne].second->setY(0);
		}
	}
	//right trigger move up or down
	if (bindedAxis == 5) {
		if (event.jaxis.value > m_triggerDeadZone)
		{
			*m_triggerValues[whichOne].second = abs(event.jaxis.value);
		}
		else
		{
			*m_triggerValues[whichOne].second = 0;
		}
	}

	float lastLx = m_joystickValues[whichOne].first->getX(), lastLy = m_joystickValues[whichOne].first->getY();

	if ((lastLx == 0 && lastLy >= 0.99) || (lastLx == 0 && lastLy <= -0.99) ||
		(lastLx >= 0.99 && lastLy == 0) || (lastLx <= -0.99 && lastLy == 0)) {
		lastLStickValue_[whichOne].x = lastLx;
		lastLStickValue_[whichOne].y = lastLy;
	}
}

void InputHandler::onJoyButtonChange(SDL_Event& event, ButtonState just) {
	if (just == JustDown)
		isButtonDownEvent_ = true;
	else
		isButtonUpEvent_ = true;

	int whichOne = event.jaxis.which;
	cout << "Button down on SYSTEM " << whichOne << endl;

	Uint8 i = 0;
	bool bindFound = false;
	int bindedButton;
	//hay que buscar el botón al que se corresponde
	//porque SDL es una librería maravillosa y super intuitiva
	while (!bindFound && i < SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_MAX) {
		SDL_GameControllerButtonBind b = SDL_GameControllerGetBindForButton(m_gameControllers[whichOne], (SDL_GameControllerButton)i);
		if (b.value.button == event.cbutton.button) {
			bindedButton = i;
			bindFound = true;
		}
		i++;
	}
	m_buttonStates[whichOne][bindedButton] = just;
}

bool InputHandler::mapJoystick(SDL_GameController* ctrl, json mapData) {
	const int bytes = 33;
	const int bits = bytes * 8;
	char guid[bits] = {};
	SDL_Joystick* joy = SDL_GameControllerGetJoystick(ctrl);
	SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joy), guid, bits);

	if (!(mapData[guid]).is_null()) {
		string mapString = mapData[guid];
		int res = SDL_GameControllerAddMapping(mapString.c_str());

		return res == 1;
	}
	else {
		return false;
	}
	return false;
}

inline void InputHandler::onControllerAddedEvent(const SDL_Event& event)
{
	if (numControllers_ < 4)
	{
		int idOnExit = m_gameControllers.size();//su id fisica sera el lugar en el que lo inicialice SDL
		int gameId; //Su id del juego sera el id que lea de la id fisica
		if (disconnectedGameControllers_.empty()) {
			initialiseNewController(event.cdevice.which);

			//si es un mando nuevo, se corresponden
			gameId = idOnExit;
		}
		else {
			debugFlag_ReconectedController = true;
			int id = disconnectedGameControllers_.front();
			initialiseNewController(event.cdevice.which);
			disconnectedGameControllers_.pop();

			//si es un mando reconectado, el mando de sistema que habia perdido su input fisico leera del nuevo mando fisico
			gameId = id;
		}
		numControllers_++;
		systemToGameCtrlId.emplace(idOnExit, gameId);
		gameToSystemCtrlId[gameId] = idOnExit;
	}
}

inline void InputHandler::onControllerRemovedEvent(const SDL_Event& event)
{
	numControllers_--;
	int gameId;
	auto it = systemToGameCtrlId.find(event.cdevice.which);
	if (it != systemToGameCtrlId.end()) {
		disconnectedGameControllers_.push(it->second);
		systemToGameCtrlId.erase(it);
	}
	else {
		//se ha desconectado un mando no inicializado
		//eso es malo.
	}
}

inline void InputHandler::initialiseNewController(int idOnEnter)
{
	SDL_GameController* gameCtrl = SDL_GameControllerOpen(idOnEnter);
	if (gameCtrl)
	{
		//cout << "--------------" << endl;
		//cout << SDL_GameControllerName(gameCtrl) << endl;
		//cout << SDL_GameControllerMapping(gameCtrl) << endl;
		m_gameControllers.push_back(gameCtrl);
		m_joystickValues.push_back(std::make_pair(new
			Vector2D(0, 0), new Vector2D(0, 0))); // add our pair
		m_triggerValues.push_back(std::make_pair
		(new double(0.0), new double(0.0)));

		lastLStickValue_.push_back(b2Vec2(0, 0));

		std::vector<ButtonState> tempButtons;
		for (int j = 0; j < SDL_CONTROLLER_BUTTON_MAX; j++)
		{
			tempButtons.push_back(Up);
		}
		m_buttonStates.push_back(tempButtons);
	}
	else
	{
		std::cout << SDL_GetError();
	}
	SDL_JoystickEventState(SDL_ENABLE);
	m_bJoysticksInitialised = true;

	//std::cout << "Initialised " << m_gameControllers.size() << " joystick(s)";
}

bool InputHandler::isButtonJustUp(int gameCtrl, SDL_GameControllerButton b)
{
	int ctrl = gameToSystemCtrlId[gameCtrl];
	if (ctrl >= m_gameControllers.size())
		return false;
	return(isButtonUpEvent_ && m_buttonStates[ctrl][b] == JustUp);
}

bool InputHandler::isButtonJustDown(int gameCtrl, SDL_GameControllerButton b)
{
	int ctrl = gameToSystemCtrlId[gameCtrl];
	if (ctrl >= m_gameControllers.size())
		return false;
	return(isButtonDownEvent_ && m_buttonStates[ctrl][b] == JustDown);
}

bool InputHandler::isButtonDown(int gameCtrl, SDL_GameControllerButton b)
{
	int ctrl = gameToSystemCtrlId[gameCtrl];

	if (ctrl >= m_gameControllers.size())
		return false;
	return(m_buttonStates[ctrl][b] == Down);
}

bool InputHandler::isButtonUp(int gameCtrl, SDL_GameControllerButton b)
{
	int ctrl = gameToSystemCtrlId[gameCtrl];
	if (ctrl >= m_gameControllers.size())
		return false;
	return(m_buttonStates[ctrl][b] == Up);
}

b2Vec2 InputHandler::getStickDir(int gameCtrl, GAMEPADSTICK stick) {
	int ctrl = gameToSystemCtrlId[gameCtrl];
	if (ctrl >= m_gameControllers.size())
		return b2Vec2(0, 0);
	Vector2D aux;
	if (stick == LEFTSTICK)
		aux = *m_joystickValues[ctrl].first;
	else
		aux = *m_joystickValues[ctrl].second;

	aux = aux.normalize();
	return b2Vec2(aux.getX(), aux.getY());
}

b2Vec2 InputHandler::getLastStickDir(int gameCtrl, GAMEPADSTICK stick) {
	int ctrl = gameToSystemCtrlId[gameCtrl];
	if (ctrl >= m_gameControllers.size())
		return b2Vec2(0, 0);
	if (stick == LEFTSTICK)
		return lastLStickValue_[ctrl];
	else
		return b2Vec2(0, 0);
}