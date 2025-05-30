// PolyNova3D (version 3.3)
/***************************************************************
 Copyright (C) 1999 Novasoft Consulting

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License as published by the Free Software Foundation; either
 version 2 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.

 You should have received a copy of the GNU Library General Public
 License along with this library; if not, write to the Free
 Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *****************************************************************/

#include "poly_nova.h"

#define JOYSTICK_PORT_DEFAULT 2

static SDL_Joystick *joystick = NULL;

InputManager::InputManager() {
	joystick = NULL;
}

InputManager::~InputManager() {
	if (joystick) {
		SDL_JoystickClose(joystick);
		joystick = NULL;
	}

	SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

void InputManager::init() {
	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) == -1) {
		fatalError("Could not initialize the input subsystem: %s\n", SDL_GetError());
	}

	logMessage("Looking for a joystick....\n");
	if (SDL_NumJoysticks() < 1) {
		fatalError("Could not find any joysticks");
	}

	SDL_JoystickEventState(SDL_ENABLE);

	// Read the joystick port to use in from the INI file (probably just 1 or 2, we will zero index it)
	int port = g_worldManager->getYamlish()->getInt("input.joystick.port", JOYSTICK_PORT_DEFAULT);
	if (port < 1) {
		fatalError("The joystick port must have be greater than zero");
	}

	joystick = SDL_JoystickOpen(port - 1);
	if (!joystick) {
		fatalError("Could not open the joystick using port %d\n", port);
	}

	if (SDL_JoystickNumButtons(joystick) < 2) {
		fatalError("The joystick must have at least two buttons");
	}

	logMessage("Using the joystick: %s in port %d\n", SDL_JoystickName(1), port);
}




