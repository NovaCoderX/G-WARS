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

#define TITLE_VERTICAL_POSITION 54
#define FIRST_CONTROL_VERTICAL_POSITION 30
#define CONTROL_VERTICAL_SPACING 20

enum MenuControls {
	NEW_GAME_BUTTON = 0,
	RESUME_GAME_BUTTON = 1,
	HIGH_SCORE_BUTTON = 2,
	OPTIONS_BUTTON = 3,
	QUIT_BUTTON = 4
};

MainMenu::MainMenu(MenuState* menuState) : BaseMenu(menuState) {
	title = NULL;
	currentSelection = NEW_GAME_BUTTON;
}

MainMenu::~MainMenu() {
	if (title) {
		delete title;
		title = NULL;
	}

    for (Control* control : controls) {
        delete control;
    }

    controls.clear();
}

void MainMenu::init() {
	logMessage("Creating the main menu...\n");

	float x = 0;
	float y = TITLE_VERTICAL_POSITION;
	float z = 0;
	NovaColor titleColor = DEFAULT_TEXT_COLOR;
	title = new MenuTitle(menuState, "G-WARS", titleColor, NovaVertex(x, y, z));

	// Create the controls.
	NovaColor buttonColor = NovaColor(154, 26, 214);
	NovaColor textColor = NovaColor(255, 255, 255);
	y = FIRST_CONTROL_VERTICAL_POSITION;

	controls.push_back(new Button(menuState, buttonColor, "NEW GAME", textColor, NovaVertex(x, y, z)));
	buttonColor.rebase(90);
	y -= CONTROL_VERTICAL_SPACING;

	controls.push_back(new Button(menuState, buttonColor, "RESUME", textColor, NovaVertex(x, y, z)));
	buttonColor.rebase(90);
	y -= CONTROL_VERTICAL_SPACING;

	controls.push_back(new Button(menuState, buttonColor, "HIGH SCORES", textColor, NovaVertex(x, y, z)));
	buttonColor.rebase(90);
	y -= CONTROL_VERTICAL_SPACING;

	controls.push_back(new Button(menuState, buttonColor, "OPTIONS", textColor, NovaVertex(x, y, z)));
	buttonColor.rebase(90);
	y -= CONTROL_VERTICAL_SPACING;

	controls.push_back(new Button(menuState, buttonColor, "QUIT GAME", textColor, NovaVertex(x, y, z)));
}

void MainMenu::syncCurrentSelection() {
	// Sync the controls.
	for (Control* control : controls) {
		control->setSelected(false);
	}

	controls[currentSelection]->setSelected(true);
}

void MainMenu::handleKeyDown(SDL_keysym *keysym) {
	bool shortcutKeyUsed = false;

	// Alt-enter toggles full screen mode
	if ((keysym->sym == SDLK_RETURN) && (keysym->mod & KMOD_ALT)) {
		g_worldManager->toggleFullScreen();
		shortcutKeyUsed = true;
	}

	if (!shortcutKeyUsed) {
		// Normal processing.
		switch (keysym->sym) {
		case SDLK_ESCAPE:
			currentSelection = QUIT_BUTTON;
			syncCurrentSelection();
			break;
		case SDLK_UP:
			selectionUp();
			break;
		case SDLK_DOWN:
			selectionDown();
			break;
		case SDLK_RETURN:
			selectionChosen();
			break;
		default:
			break;
		}
	}
}

void MainMenu::handleKeyUp(SDL_keysym *keysym) {
	// Empty.
}

void MainMenu::processInput() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_JOYAXISMOTION:
			if (event.jaxis.axis == 1) {
				// Vertical movement.
				if (event.jaxis.value < 0) {
					selectionUp();
				} else if (event.jaxis.value > 0) {
					selectionDown();
				}
			}
			break;
		case SDL_JOYBUTTONDOWN:
			if (event.jbutton.button == 0) {
				selectionChosen();
			}
			break;
		case SDL_KEYDOWN:
			handleKeyDown(&event.key.keysym);
			break;
		case SDL_KEYUP:
			handleKeyUp(&event.key.keysym);
			break;
		case SDL_QUIT:
			exit(EXIT_SUCCESS);
			break;
		}
	}
}

void MainMenu::update(float elapsedTime) {
	for (Control* control : controls) {
		control->update(elapsedTime);
	}
}

void MainMenu::draw() {
	title->draw();

	for (Control* control : controls) {
		control->draw();
	}
}

void MainMenu::reset() {
	currentSelection = 0;
}

void MainMenu::enterState() {
	// Update the selected and disabled controls.
	if (!g_worldManager->isGameInProgress()) {
		controls[RESUME_GAME_BUTTON]->setDisabled(true);
	} else {
		controls[RESUME_GAME_BUTTON]->setDisabled(false);

		// Game in progress, change the selected button.
		if (currentSelection == NEW_GAME_BUTTON) {
			currentSelection = RESUME_GAME_BUTTON;
		}
	}

	syncCurrentSelection();
}

void MainMenu::selectionUp() {
	g_worldManager->startSound(UI_CHANGE);

	currentSelection--;
	if (currentSelection < NEW_GAME_BUTTON) {
		currentSelection = QUIT_BUTTON;
	}

	// If the control is disabled we should skip it.
	if (controls[currentSelection]->isDisabled()) {
		currentSelection--;
	}

	syncCurrentSelection();
}

void MainMenu::selectionDown() {
	g_worldManager->startSound(UI_CHANGE);

	currentSelection++;
	if (currentSelection > QUIT_BUTTON) {
		currentSelection = NEW_GAME_BUTTON;
	}

	// If the control is disabled we should skip it.
	if (controls[currentSelection]->isDisabled()) {
		currentSelection++;
	}

	syncCurrentSelection();
}

void MainMenu::selectionChosen() {
	int channel = g_worldManager->startSound(UI_CONFIRM);
	switch (currentSelection) {
	case NEW_GAME_BUTTON:
		// New game (reset state).
		g_worldManager->setActiveState(PLAY_STATE, true);
		break;

	case RESUME_GAME_BUTTON:
		// Resume a paused game.
		g_worldManager->setActiveState(PLAY_STATE);
		break;

	case HIGH_SCORE_BUTTON:
		menuState->setActiveMenu(HIGHSCORE_MENU, true);
		break;

	case OPTIONS_BUTTON:
		menuState->setActiveMenu(OPTIONS_MENU, true);
		break;

	case QUIT_BUTTON:
		// Let the UI sound finish playing.
		Uint32 startTime = SDL_GetTicks();
		while (g_worldManager->isSoundPlaying(channel)) {
			if ((SDL_GetTicks() - startTime) > 3000) {
				break;
			}
		}

		g_worldManager->save();
		exit(EXIT_SUCCESS);
		break;
	}
}

