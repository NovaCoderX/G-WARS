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


#define NUM_PANEL_CHARACTERS 8
#define NUM_PANEL_DIGITS 9

static std::string formatStringWithCommas(int value) {
	char buffer[20];

	std::snprintf(buffer, sizeof(buffer), "%d", value);
	std::string numStr(buffer);
	int insertPosition = numStr.length() - 3;

	while (insertPosition > 0) {
		numStr.insert(insertPosition, ",");
		insertPosition -= 3;
	}

	return numStr;
}

HighScorePanel::HighScorePanel(PlayState* playState) :
		HudPanel(playState) {
	// Empty.
}

HighScorePanel::~HighScorePanel() {
	for (CharacterSprite* character : characters) {
		delete character;
	}

	characters.clear();

	for (CharacterDigit* digit : digits) {
		delete digit;
	}

	digits.clear();
}

void HighScorePanel::init() {
	// Setup the score text.
	for (int i = 0; i < NUM_PANEL_CHARACTERS; i++) {
		characters.push_back(new CharacterSprite(playState));
	}

	characters[0]->init(g_worldManager->getSpriteDefinition("small_letter_h"), textColor);
	characters[1]->init(g_worldManager->getSpriteDefinition("small_letter_i"), textColor);
	characters[2]->init(g_worldManager->getSpriteDefinition("small_dash_separator"), textColor);
	characters[3]->init(g_worldManager->getSpriteDefinition("small_letter_s"), textColor);
	characters[4]->init(g_worldManager->getSpriteDefinition("small_letter_c"), textColor);
	characters[5]->init(g_worldManager->getSpriteDefinition("small_letter_o"), textColor);
	characters[6]->init(g_worldManager->getSpriteDefinition("small_letter_r"), textColor);
	characters[7]->init(g_worldManager->getSpriteDefinition("small_letter_e"), textColor);

	// Position of the first letter.
	int x = -65;
	int y = 65;

	// Position the letters then transform and project in advance.
	for (int i = 0; i < NUM_PANEL_CHARACTERS; i++) {
		characters[i]->moveTo(x, y, 0);
		characters[i]->transform();

		if (i == 1) {
			// Need to change spacing for the separator.
			x += 3;
		} else {
			x += 4;
		}
	}

	// Setup the score digits.
	for (int i = 0; i < NUM_PANEL_DIGITS; i++) {
		digits.push_back(new CharacterDigit(playState));
		digits[i]->init(textColor);
	}

	// Position of the first digit.
	x = -65;
	y = 60;

	// Position the digits then transform and project in advance.
	for (int i = 0; i < NUM_PANEL_DIGITS; i++) {
		digits[i]->moveTo(x, y, 0);
		digits[i]->transform();
		x += CHARACTER_DIGIT_WIDTH;
	}
}

void HighScorePanel::update(float elapsedTime) {
	std::string formattedNumber = formatStringWithCommas(playState->getPlayer()->getHighScore());

	// Need to check for overflow.
	if (formattedNumber.size() > NUM_PANEL_DIGITS) {
		formattedNumber.resize(NUM_PANEL_DIGITS);
	}

	for (std::string::size_type i = 0; i < formattedNumber.size(); i++) {
		if (std::isdigit(formattedNumber[i])) {
			int number = std::stoi(std::string(1, formattedNumber[i]));
			digits[i]->setVisible(number);
		} else {
			// Comma separator.
			digits[i]->setVisible(COMMA_SEPARATOR_DIGIT);
		}
	}

	// Need to reformat the digits to take into account comma spacing.
	int x = -65;
	int y = 60;

	bool shortSpacing = false;

	for (int i = 1; i < NUM_PANEL_DIGITS; i++) {
		if (!digits[i]->isVisible()) {
			break; // No point continuing.
		}

		if (digits[i]->getActiveDigit() == COMMA_SEPARATOR_DIGIT) {
			x += (CHARACTER_DIGIT_WIDTH - 2);
			shortSpacing = true;
		} else {
			if (shortSpacing) {
				x += (CHARACTER_DIGIT_WIDTH - 1);

				// Reset.
				shortSpacing = false;
			} else {
				// Normal spacing.
				x += CHARACTER_DIGIT_WIDTH;
			}
		}

		digits[i]->moveTo(x, y, 0);
		digits[i]->transform();
	}

	// No longer 'dirty'.
	this->setDirty(false);
}

void HighScorePanel::draw() {
	// First draw the text.
	for (int i = 0; i < NUM_PANEL_CHARACTERS; i++) {
		characters[i]->draw();
	}

	// Then draw the digits.
	for (int i = 0; i < NUM_PANEL_DIGITS; i++) {
		if (digits[i]->isVisible()) {
			digits[i]->draw();
		} else {
			break; // No point continuing.
		}
	}
}
