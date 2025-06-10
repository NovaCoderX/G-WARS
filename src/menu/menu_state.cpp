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

#define NUMBER_OF_ALPHA_CHARACTERS 26
#define NUMBER_OF_NUMERICAL_CHARACTERS 10
#define NUMBER_OF_SPECIAL_CHARACTERS 3

static Uint32 lastFrameTime = 0;

MenuState::MenuState() {
	camera = NULL;
	smallAlphaCharacters = NULL;
	largeAlphaCharacters = NULL;
	smallSpecialCharactes = NULL;
	largeSpecialCharactes = NULL;
	numericalCharactes = NULL;
	activeMenu = NULL;
	mainMenu = NULL;
	highScoreMenu = NULL;
	optionsMenu = NULL;
	menuBackground = NULL;
}

MenuState::~MenuState() {
	if (camera) {
		delete camera;
		camera = NULL;
	}

	if (smallAlphaCharacters) {
		delete[] smallAlphaCharacters;
		smallAlphaCharacters = NULL;
	}

	if (largeAlphaCharacters) {
		delete[] largeAlphaCharacters;
		largeAlphaCharacters = NULL;
	}

	if (smallSpecialCharactes) {
		delete[] smallSpecialCharactes;
		smallSpecialCharactes = NULL;
	}

	if (largeSpecialCharactes) {
		delete[] largeSpecialCharactes;
		largeSpecialCharactes = NULL;
	}

	if (numericalCharactes) {
		delete[] numericalCharactes;
		numericalCharactes = NULL;
	}

	if (mainMenu) {
		delete mainMenu;
		mainMenu = NULL;
	}

	if (highScoreMenu) {
		delete highScoreMenu;
		highScoreMenu = NULL;
	}

	if (optionsMenu) {
		delete optionsMenu;
		optionsMenu = NULL;
	}

	if (menuBackground) {
		delete menuBackground;
		menuBackground = NULL;
	}
}

SpriteDefinition* MenuState::getAlphaCharacter(int index, CharacterSize size) {
	if (index < 0) {
		fatalError("Character index out of bounds\n");
	}

	if (index >= NUMBER_OF_ALPHA_CHARACTERS) {
		fatalError("Character index out of bounds\n");
	}

	if (size == SMALL_CHARACTER) {
		return smallAlphaCharacters[index];
	} else {
		return largeAlphaCharacters[index];
	}
}

SpriteDefinition* MenuState::getSpecialCharacter(int index, CharacterSize size) {
	if (index < 0) {
		fatalError("Character index out of bounds\n");
	}

	if (index >= NUMBER_OF_SPECIAL_CHARACTERS) {
		fatalError("Character index out of bounds\n");
	}

	if (size == SMALL_CHARACTER) {
		return smallSpecialCharactes[index];
	} else {
		return largeSpecialCharactes[index];
	}
}

SpriteDefinition* MenuState::getNumericalCharacter(int index) {
	if (index < 0) {
		fatalError("Number index out of bounds\n");
	}

	if (index >= NUMBER_OF_NUMERICAL_CHARACTERS) {
		fatalError("Number index out of bounds\n");
	}

	return numericalCharactes[index];
}

void MenuState::setActiveMenu(MenuType menu, bool reset) {
	if (activeMenu) {
		activeMenu->leaveState();
	}

	if (menu == MAIN_MENU) {
		if (!mainMenu) {
			mainMenu = new MainMenu(this);
			mainMenu->init();
			reset = false; // Because it was just reset.
		}

		activeMenu = mainMenu;
		if (reset) {
			activeMenu->reset();
		}
	}

	if (menu == HIGHSCORE_MENU) {
		if (!highScoreMenu) {
			highScoreMenu = new HighScoreMenu(this);
			highScoreMenu->init();
			reset = false; // Because it was just reset.
		}

		activeMenu = highScoreMenu;
		if (reset) {
			activeMenu->reset();
		}
	}

	if (menu == OPTIONS_MENU) {
		if (!optionsMenu) {
			optionsMenu = new OptionsMenu(this);
			optionsMenu->init();
			reset = false; // Because it was just reset.
		}

		activeMenu = optionsMenu;
		if (reset) {
			activeMenu->reset();
		}
	}

	if (activeMenu) {
		activeMenu->enterState();
	}
}

void MenuState::init() {
	camera = new Camera();

	// Create the lookup characters used by the buttons, labels and titles.
	smallAlphaCharacters = new SpriteDefinition*[NUMBER_OF_ALPHA_CHARACTERS];
	smallAlphaCharacters[0] = g_worldManager->getSpriteDefinition("small_letter_a");
	smallAlphaCharacters[1] = g_worldManager->getSpriteDefinition("small_letter_b");
	smallAlphaCharacters[2] = g_worldManager->getSpriteDefinition("small_letter_c");
	smallAlphaCharacters[3] = g_worldManager->getSpriteDefinition("small_letter_d");
	smallAlphaCharacters[4] = g_worldManager->getSpriteDefinition("small_letter_e");
	smallAlphaCharacters[5] = g_worldManager->getSpriteDefinition("small_letter_f");
	smallAlphaCharacters[6] = g_worldManager->getSpriteDefinition("small_letter_g");
	smallAlphaCharacters[7] = g_worldManager->getSpriteDefinition("small_letter_h");
	smallAlphaCharacters[8] = g_worldManager->getSpriteDefinition("small_letter_i");
	smallAlphaCharacters[9] = g_worldManager->getSpriteDefinition("small_letter_j");
	smallAlphaCharacters[10] = g_worldManager->getSpriteDefinition("small_letter_k");
	smallAlphaCharacters[11] = g_worldManager->getSpriteDefinition("small_letter_l");
	smallAlphaCharacters[12] = g_worldManager->getSpriteDefinition("small_letter_m");
	smallAlphaCharacters[13] = g_worldManager->getSpriteDefinition("small_letter_n");
	smallAlphaCharacters[14] = g_worldManager->getSpriteDefinition("small_letter_o");
	smallAlphaCharacters[15] = g_worldManager->getSpriteDefinition("small_letter_p");
	smallAlphaCharacters[16] = g_worldManager->getSpriteDefinition("small_letter_q");
	smallAlphaCharacters[17] = g_worldManager->getSpriteDefinition("small_letter_r");
	smallAlphaCharacters[18] = g_worldManager->getSpriteDefinition("small_letter_s");
	smallAlphaCharacters[19] = g_worldManager->getSpriteDefinition("small_letter_t");
	smallAlphaCharacters[20] = g_worldManager->getSpriteDefinition("small_letter_u");
	smallAlphaCharacters[21] = g_worldManager->getSpriteDefinition("small_letter_v");
	smallAlphaCharacters[22] = g_worldManager->getSpriteDefinition("small_letter_w");
	smallAlphaCharacters[23] = g_worldManager->getSpriteDefinition("small_letter_x");
	smallAlphaCharacters[24] = g_worldManager->getSpriteDefinition("small_letter_y");
	smallAlphaCharacters[25] = g_worldManager->getSpriteDefinition("small_letter_z");

	largeAlphaCharacters = new SpriteDefinition*[NUMBER_OF_ALPHA_CHARACTERS];
	largeAlphaCharacters[0] = g_worldManager->getSpriteDefinition("large_letter_a");
	largeAlphaCharacters[1] = g_worldManager->getSpriteDefinition("large_letter_b");
	largeAlphaCharacters[2] = g_worldManager->getSpriteDefinition("large_letter_c");
	largeAlphaCharacters[3] = g_worldManager->getSpriteDefinition("large_letter_d");
	largeAlphaCharacters[4] = g_worldManager->getSpriteDefinition("large_letter_e");
	largeAlphaCharacters[5] = g_worldManager->getSpriteDefinition("large_letter_f");
	largeAlphaCharacters[6] = g_worldManager->getSpriteDefinition("large_letter_g");
	largeAlphaCharacters[7] = g_worldManager->getSpriteDefinition("large_letter_h");
	largeAlphaCharacters[8] = g_worldManager->getSpriteDefinition("large_letter_i");
	largeAlphaCharacters[9] = g_worldManager->getSpriteDefinition("large_letter_j");
	largeAlphaCharacters[10] = g_worldManager->getSpriteDefinition("large_letter_k");
	largeAlphaCharacters[11] = g_worldManager->getSpriteDefinition("large_letter_l");
	largeAlphaCharacters[12] = g_worldManager->getSpriteDefinition("large_letter_m");
	largeAlphaCharacters[13] = g_worldManager->getSpriteDefinition("large_letter_n");
	largeAlphaCharacters[14] = g_worldManager->getSpriteDefinition("large_letter_o");
	largeAlphaCharacters[15] = g_worldManager->getSpriteDefinition("large_letter_p");
	largeAlphaCharacters[16] = g_worldManager->getSpriteDefinition("large_letter_q");
	largeAlphaCharacters[17] = g_worldManager->getSpriteDefinition("large_letter_r");
	largeAlphaCharacters[18] = g_worldManager->getSpriteDefinition("large_letter_s");
	largeAlphaCharacters[19] = g_worldManager->getSpriteDefinition("large_letter_t");
	largeAlphaCharacters[20] = g_worldManager->getSpriteDefinition("large_letter_u");
	largeAlphaCharacters[21] = g_worldManager->getSpriteDefinition("large_letter_v");
	largeAlphaCharacters[22] = g_worldManager->getSpriteDefinition("large_letter_w");
	largeAlphaCharacters[23] = g_worldManager->getSpriteDefinition("large_letter_x");
	largeAlphaCharacters[24] = g_worldManager->getSpriteDefinition("large_letter_y");
	largeAlphaCharacters[25] = g_worldManager->getSpriteDefinition("large_letter_z");

	smallSpecialCharactes = new SpriteDefinition*[NUMBER_OF_SPECIAL_CHARACTERS];
	smallSpecialCharactes[DASH_CHARACTER_INDEX] = g_worldManager->getSpriteDefinition("small_dash_separator");
	smallSpecialCharactes[COLON_CHARACTER_INDEX] = g_worldManager->getSpriteDefinition("small_colon_separator");
	smallSpecialCharactes[CURSOR_CHARACTER_INDEX] = g_worldManager->getSpriteDefinition("small_cursor");

	largeSpecialCharactes = new SpriteDefinition*[NUMBER_OF_SPECIAL_CHARACTERS];
	largeSpecialCharactes[DASH_CHARACTER_INDEX] = g_worldManager->getSpriteDefinition("large_dash_separator");
	largeSpecialCharactes[COLON_CHARACTER_INDEX] = g_worldManager->getSpriteDefinition("large_colon_separator");
	largeSpecialCharactes[CURSOR_CHARACTER_INDEX] = g_worldManager->getSpriteDefinition("large_cursor");

	numericalCharactes = new SpriteDefinition*[NUMBER_OF_NUMERICAL_CHARACTERS];
	numericalCharactes[0] = g_worldManager->getSpriteDefinition("number_zero");
	numericalCharactes[1] = g_worldManager->getSpriteDefinition("number_one");
	numericalCharactes[2] = g_worldManager->getSpriteDefinition("number_two");
	numericalCharactes[3] = g_worldManager->getSpriteDefinition("number_three");
	numericalCharactes[4] = g_worldManager->getSpriteDefinition("number_four");
	numericalCharactes[5] = g_worldManager->getSpriteDefinition("number_five");
	numericalCharactes[6] = g_worldManager->getSpriteDefinition("number_six");
	numericalCharactes[7] = g_worldManager->getSpriteDefinition("number_seven");
	numericalCharactes[8] = g_worldManager->getSpriteDefinition("number_eight");
	numericalCharactes[9] = g_worldManager->getSpriteDefinition("number_nine");

	menuBackground = new MenuBackground(this);
	menuBackground->init();

	logMessage("Finished setting up the menu state\n");
}

void MenuState::processInput() {
	activeMenu->processInput();
}

void MenuState::update() {
	Uint32 currentFrameTime = SDL_GetTicks();

	float elapsedTime = (currentFrameTime - lastFrameTime) / 1000.0;

	menuBackground->update(elapsedTime);
	activeMenu->update(elapsedTime);

	// Store.
	lastFrameTime = currentFrameTime;
}

void MenuState::draw() {
	menuBackground->draw();
	activeMenu->draw();
}

void MenuState::reset() {
	activeMenu->reset();
}

void MenuState::enterState() {
	// This will create the main menu screen for the first time.
	this->setActiveMenu(MAIN_MENU);

	// Update to the current time.
	lastFrameTime = SDL_GetTicks();
}

void MenuState::leaveState() {
	if (activeMenu) {
		activeMenu->leaveState();
	}
}

