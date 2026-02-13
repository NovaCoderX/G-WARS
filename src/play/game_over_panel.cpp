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

#define NUM_TOP_CHARACTERS 8
#define FIRST_WORD_LENGTH 4

GameOverPanel::GameOverPanel(PlayState* playState) : HudPanel(playState) {
	visible = false;
	animationComplete = false;
}

GameOverPanel::~GameOverPanel() {
    for (CharacterSprite* character : characters) {
        delete character;
    }

    characters.clear();
}

void GameOverPanel::init() {
	// Setup the game over message.
	for (int i = 0; i < NUM_TOP_CHARACTERS; i++) {
		characters.push_back(new CharacterSprite(playState));
	}

	characters[0]->init(g_worldManager->getSpriteDefinition("large_letter_g"), textColor);
	characters[1]->init(g_worldManager->getSpriteDefinition("large_letter_a"), textColor);
	characters[2]->init(g_worldManager->getSpriteDefinition("large_letter_m"), textColor);
	characters[3]->init(g_worldManager->getSpriteDefinition("large_letter_e"), textColor);
	characters[4]->init(g_worldManager->getSpriteDefinition("large_letter_o"), textColor);
	characters[5]->init(g_worldManager->getSpriteDefinition("large_letter_v"), textColor);
	characters[6]->init(g_worldManager->getSpriteDefinition("large_letter_e"), textColor);
	characters[7]->init(g_worldManager->getSpriteDefinition("large_letter_r"), textColor);

	// Position of the first letter.
	int x = -60;

	// First word.
	for (int i = 0; i < FIRST_WORD_LENGTH; i++) {
		characters[i]->moveTo(x, 0, 0);
		x += LARGE_CHARACTER_WIDTH;
	}

	// Add a space in the middle of the words.
	x += LARGE_CHARACTER_WIDTH;

	// Second word.
	for (int i = FIRST_WORD_LENGTH; i < NUM_TOP_CHARACTERS; i++) {
		characters[i]->moveTo(x, 0, 0);
		x += LARGE_CHARACTER_WIDTH;
	}

	// Then we need to transform and project each letter in advance.
	for (int i = 0; i < NUM_TOP_CHARACTERS; i++) {
		characters[i]->transform();
	}

	// Setup up the fade in colors.
	currentTextColor = this->textColor;
	currentTextColor.rebase(30);
}

void GameOverPanel::update(float elapsedTime) {
	animationComplete = currentTextColor.brighten((elapsedTime / 90), textColor);
}

void GameOverPanel::draw() {
	for (int i = 0; i < NUM_TOP_CHARACTERS; i++) {
		characters[i]->setCurrentColor(currentTextColor);
		characters[i]->draw();
	}
}
