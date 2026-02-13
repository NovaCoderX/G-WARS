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

#include <iostream>
#include <string>
#include <sstream>

#define NUM_TOP_CHARACTERS 3
#define MAX_NUM_VALUE 9

PlayerStatsPanel::PlayerStatsPanel(PlayState* playState) : HudPanel(playState) {
	lives = NULL;
	bombs = NULL;
}

PlayerStatsPanel::~PlayerStatsPanel() {
    for (CharacterSprite* character : characters) {
        delete character;
    }

    characters.clear();

	if (lives) {
		delete lives;
		lives = NULL;
	}

	if (bombs) {
		delete bombs;
		bombs = NULL;
	}
}

void PlayerStatsPanel::init() {
	// Setup the stats.
	for (int i = 0; i < NUM_TOP_CHARACTERS; i++) {
		characters.push_back(new CharacterSprite(playState));
	}

	characters[0]->init(g_worldManager->getSpriteDefinition("small_letter_l"), textColor);
	characters[1]->init(g_worldManager->getSpriteDefinition("small_dash_separator"), textColor);
	characters[2]->init(g_worldManager->getSpriteDefinition("small_letter_b"), textColor);

	lives = new CharacterDigit(playState);
	lives->init(textColor);

	bombs = new CharacterDigit(playState);
	bombs->init(textColor);

	// Position the letters and digits then transform and project in advance.
	int y = 65;
	int x = -8;

	// L
	characters[0]->moveTo(x, y, 0);
	characters[0]->transform();
	x += 4;

	// L#
	lives->moveTo(x, y, 0);
	lives->transform();
	x += 4;

	// Separator
	characters[1]->moveTo(x, y, 0);
	characters[1]->transform();
	x += 4;

	// B
	characters[2]->moveTo(x, y, 0);
	characters[2]->transform();
	x += 4;

	// B#
	bombs->moveTo(x, y, 0);
	bombs->transform();
	x += 4;
}

void PlayerStatsPanel::update(float elapsedTime) {
	int numlives = playState->getPlayer()->getNumLives();

	// Need to check for overflow.
	if (numlives > MAX_NUM_VALUE) {
		numlives = MAX_NUM_VALUE;
	}

	lives->setVisible(numlives);

	int numBombs = playState->getPlayer()->getNumSmartBombs();

	// Need to check for overflow.
	if (numBombs > MAX_NUM_VALUE) {
		numBombs = MAX_NUM_VALUE;
	}

	bombs->setVisible(numBombs);

	// No longer 'dirty'.
	this->setDirty(false);
}

void PlayerStatsPanel::draw() {
	// First draw the text.
	for (int i = 0; i < NUM_TOP_CHARACTERS; i++) {
		characters[i]->draw();
	}

	// Then draw the digits.
	lives->draw();
	bombs->draw();
}
