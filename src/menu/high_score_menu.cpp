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
#define FIRST_BUTTON_VERTICAL_POSITION -50
#define FIRST_ENTRY_VERTICAL_POSITION 28
#define ENTRY_VERTICAL_SPACING 8
#define PANEL_WIDTH 100
#define PANEL_HEIGHT 70

enum MenuControls {
	RETURN_BUTTON = 0
};

HighScoreMenu::HighScoreMenu(MenuState* menuState) : BaseMenu(menuState) {
	title = NULL;
	panel = NULL;
	currentSelection = RETURN_BUTTON;
}

HighScoreMenu::~HighScoreMenu() {
	if (title) {
		delete title;
		title = NULL;
	}

	if (panel) {
		delete panel;
		panel = NULL;
	}

    for (Control* control : controls) {
        delete control;
    }

    controls.clear();

	for (HighScoreEntry* entry : entries) {
		 delete entry;
	}

	entries.clear();
}

void HighScoreMenu::init() {
	logMessage("Creating the high score menu...\n");

	float x = 0;
	float y = TITLE_VERTICAL_POSITION;
	float z = 0;
	NovaColor titleColor = DEFAULT_TEXT_COLOR;
	title = new MenuTitle(menuState, "HIGH SCORES", titleColor, NovaVertex(x, y, z));

	// Setup the background panel.
	NovaColor panelColor = NovaColor(0, 0, 0);
	NovaColor borderColor = DEFAULT_TEXT_COLOR;
	panel = new MenuPanel(menuState, PANEL_WIDTH, PANEL_HEIGHT, panelColor, borderColor, NovaVertex(0, 0, 0));

	// Create the controls.
	y = FIRST_BUTTON_VERTICAL_POSITION;
	NovaColor buttonColor = NovaColor(101, 17, 140);
	NovaColor textColor = NovaColor(255, 255, 255);
	controls.push_back(new Button(menuState, buttonColor, "RETURN", textColor, NovaVertex(x, y, z)));
}

void HighScoreMenu::syncCurrentSelection() {
	// Sync the controls.
	for (Control* control : controls) {
		control->setSelected(false);
	}

	controls[currentSelection]->setSelected(true);
}

void HighScoreMenu::handleKeyDown(SDL_keysym *keysym) {
	switch (keysym->sym) {
	case SDLK_RETURN:
		selectionChosen();
		break;
	default:
		break;
	}
}

void HighScoreMenu::handleKeyUp(SDL_keysym *keysym) {
	// Empty.
}

void HighScoreMenu::processInput() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
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

void HighScoreMenu::update(float elapsedTime) {
	controls[currentSelection]->update(elapsedTime);
}

void HighScoreMenu::draw() {
	title->draw();
	panel->draw();

	for (HighScoreEntry* entry : entries) {
		entry->draw();
	}

	for (Control* control : controls) {
		control->draw();
	}
}

void HighScoreMenu::reset() {
	currentSelection = 0;
}

void HighScoreMenu::enterState() {
	// Update the selected controls.
	currentSelection = RETURN_BUTTON;
	syncCurrentSelection();

	// Need to clear out any existing entries.
	for (HighScoreEntry* entry : entries) {
		 delete entry;
	}

	entries.clear();

	// Add the new scores.
	std::vector<HighScoreData> highScores = g_worldManager->getHighScoreHandler()->getHighScores();

	// Set the high score table layout.
	float x = 0;
	float y = FIRST_ENTRY_VERTICAL_POSITION;
	float z = 0;
	NovaColor textColor = DEFAULT_TEXT_COLOR;

	for (uint i = 0; i < highScores.size(); i++) {
		entries.push_back(new HighScoreEntry(menuState, i+1, highScores[i].name, highScores[i].score, textColor, NovaVertex(x, y, z)));
		y -= ENTRY_VERTICAL_SPACING;
	}
}

void HighScoreMenu::selectionChosen() {
	switch (currentSelection) {
	case RETURN_BUTTON:
		menuState->setActiveMenu(MAIN_MENU);
		break;
	}
}

