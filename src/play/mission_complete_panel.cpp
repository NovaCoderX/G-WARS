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

#define NUM_TOP_CHARACTERS 15
#define NUM_BOTTOM_CHARACTERS 20
#define FIRST_WORD_LENGTH 4
#define SECOND_WORD_LENGTH 6
#define THIRD_WORD_LENGTH 2
#define HIGH_COLOR_INTERVAL 30

MissionCompletePanel::MissionCompletePanel(PlayState* playState) : HudPanel(playState) {
	visible = false;
	highColor = NovaColor(227, 64, 19);
	lowColor = highColor;
	lowColor.rebase(10);
	currentTextColor = lowColor;
	increasingColor = true;
	highColorTimer = 0;
	highColorDisplayed = false;
}

MissionCompletePanel::~MissionCompletePanel() {
    for (CharacterSprite* character : topCharacters) {
        delete character;
    }

    topCharacters.clear();

    for (CharacterSprite* character : bottomCharacters) {
        delete character;
    }

    bottomCharacters.clear();
}

void MissionCompletePanel::setVisible(bool visible) {
	this->visible = visible;

	// Hide the player when displaying this message.
	playState->getPlayer()->setVisible(!visible);
}

void MissionCompletePanel::init() {
	// Setup the mission complete message.
	for (int i = 0; i < NUM_TOP_CHARACTERS; i++) {
		topCharacters.push_back(new CharacterSprite(playState));
	}

	topCharacters[0]->init(g_worldManager->getSpriteDefinition("medium_letter_c"), textColor);
	topCharacters[1]->init(g_worldManager->getSpriteDefinition("medium_letter_o"), textColor);
	topCharacters[2]->init(g_worldManager->getSpriteDefinition("medium_letter_n"), textColor);
	topCharacters[3]->init(g_worldManager->getSpriteDefinition("medium_letter_g"), textColor);
	topCharacters[4]->init(g_worldManager->getSpriteDefinition("medium_letter_r"), textColor);
	topCharacters[5]->init(g_worldManager->getSpriteDefinition("medium_letter_a"), textColor);
	topCharacters[6]->init(g_worldManager->getSpriteDefinition("medium_letter_t"), textColor);
	topCharacters[7]->init(g_worldManager->getSpriteDefinition("medium_letter_u"), textColor);
	topCharacters[8]->init(g_worldManager->getSpriteDefinition("medium_letter_l"), textColor);
	topCharacters[9]->init(g_worldManager->getSpriteDefinition("medium_letter_a"), textColor);
	topCharacters[10]->init(g_worldManager->getSpriteDefinition("medium_letter_t"), textColor);
	topCharacters[11]->init(g_worldManager->getSpriteDefinition("medium_letter_i"), textColor);
	topCharacters[12]->init(g_worldManager->getSpriteDefinition("medium_letter_o"), textColor);
	topCharacters[13]->init(g_worldManager->getSpriteDefinition("medium_letter_n"), textColor);
	topCharacters[14]->init(g_worldManager->getSpriteDefinition("medium_letter_s"), textColor);

	// Position the top letters.
	int x = -60;
	int y = 20;
	for (int i = 0; i < NUM_TOP_CHARACTERS; i++) {
		topCharacters[i]->moveTo(x, y, 0);
		x += MEDIUM_CHARACTER_WIDTH;
	}

	for (int i = 0; i < NUM_BOTTOM_CHARACTERS; i++) {
		bottomCharacters.push_back(new CharacterSprite(playState));
	}

	bottomCharacters[0]->init(g_worldManager->getSpriteDefinition("small_letter_y"), textColor);
	bottomCharacters[1]->init(g_worldManager->getSpriteDefinition("small_letter_o"), textColor);
	bottomCharacters[2]->init(g_worldManager->getSpriteDefinition("small_letter_u"), textColor);
	bottomCharacters[3]->init(g_worldManager->getSpriteDefinition("small_letter_r"), textColor);
	bottomCharacters[4]->init(g_worldManager->getSpriteDefinition("small_letter_m"), textColor);
	bottomCharacters[5]->init(g_worldManager->getSpriteDefinition("small_letter_i"), textColor);
	bottomCharacters[6]->init(g_worldManager->getSpriteDefinition("small_letter_s"), textColor);
	bottomCharacters[7]->init(g_worldManager->getSpriteDefinition("small_letter_s"), textColor);
	bottomCharacters[8]->init(g_worldManager->getSpriteDefinition("small_letter_o"), textColor);
	bottomCharacters[9]->init(g_worldManager->getSpriteDefinition("small_letter_n"), textColor);
	bottomCharacters[10]->init(g_worldManager->getSpriteDefinition("small_letter_i"), textColor);
	bottomCharacters[11]->init(g_worldManager->getSpriteDefinition("small_letter_s"), textColor);
	bottomCharacters[12]->init(g_worldManager->getSpriteDefinition("small_letter_c"), textColor);
	bottomCharacters[13]->init(g_worldManager->getSpriteDefinition("small_letter_o"), textColor);
	bottomCharacters[14]->init(g_worldManager->getSpriteDefinition("small_letter_m"), textColor);
	bottomCharacters[15]->init(g_worldManager->getSpriteDefinition("small_letter_p"), textColor);
	bottomCharacters[16]->init(g_worldManager->getSpriteDefinition("small_letter_l"), textColor);
	bottomCharacters[17]->init(g_worldManager->getSpriteDefinition("small_letter_e"), textColor);
	bottomCharacters[18]->init(g_worldManager->getSpriteDefinition("small_letter_t"), textColor);
	bottomCharacters[19]->init(g_worldManager->getSpriteDefinition("small_letter_e"), textColor);

	// Position the bottom letters.
	x = -50;
	y = 0;

	// First word.
	for (int i = 0; i < FIRST_WORD_LENGTH; i++) {
		bottomCharacters[i]->moveTo(x, y, 0);
		x += SMALL_CHARACTER_WIDTH;
	}

	// Add a space in the middle of the words.
	x += SMALL_CHARACTER_WIDTH;

	// Second word.
	for (int i = FIRST_WORD_LENGTH; i < (FIRST_WORD_LENGTH + SECOND_WORD_LENGTH); i++) {
		bottomCharacters[i]->moveTo(x, y, 0);
		x += SMALL_CHARACTER_WIDTH;
	}

	// Add a space in the middle of the words.
	x += SMALL_CHARACTER_WIDTH;

	// Third word.
	for (int i = (FIRST_WORD_LENGTH + SECOND_WORD_LENGTH); i < (FIRST_WORD_LENGTH + SECOND_WORD_LENGTH + THIRD_WORD_LENGTH); i++) {
		bottomCharacters[i]->moveTo(x, y, 0);
		x += SMALL_CHARACTER_WIDTH;
	}

	// Add a space in the middle of the words.
	x += SMALL_CHARACTER_WIDTH;

	// Forth word.
	for (int i = (FIRST_WORD_LENGTH + SECOND_WORD_LENGTH + THIRD_WORD_LENGTH); i < NUM_BOTTOM_CHARACTERS; i++) {
		bottomCharacters[i]->moveTo(x, y, 0);
		x += SMALL_CHARACTER_WIDTH;
	}

	// Then we need to transform and project each letter in advance.
	for (int i = 0; i < NUM_TOP_CHARACTERS; i++) {
		topCharacters[i]->transform();
	}

	for (int i = 0; i < NUM_BOTTOM_CHARACTERS; i++) {
		bottomCharacters[i]->transform();
	}
}

void MissionCompletePanel::update(float elapsedTime) {
	// Do some simple color cycling.
	if (highColorDisplayed) {
		highColorTimer += elapsedTime;
		if (highColorTimer >= HIGH_COLOR_INTERVAL) {
			// Reset.
			highColorDisplayed = false;
			highColorTimer = 0;
		}
	} else {
		if (increasingColor) {
			if (currentTextColor.brighten((elapsedTime / 30), highColor)) {
				increasingColor = false;

				// Stay at the highest color for a few ticks.
				highColorDisplayed = true;
			}
		} else {
			if (currentTextColor.fade((elapsedTime / 30), lowColor)) {
				// Animation complete.
				increasingColor = true;
				this->setVisible(false);
			}
		}
	}
}

void MissionCompletePanel::draw() {
	for (int i = 0; i < NUM_TOP_CHARACTERS; i++) {
		topCharacters[i]->setCurrentColor(currentTextColor);
		topCharacters[i]->draw();
	}

	for (int i = 0; i < NUM_BOTTOM_CHARACTERS; i++) {
		bottomCharacters[i]->setCurrentColor(currentTextColor);
		bottomCharacters[i]->draw();
	}
}
