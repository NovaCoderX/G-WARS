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

#include <algorithm>

#define NUM_SCORE_DIGITS 9

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

HighScoreEntry::HighScoreEntry(MenuState *menuState, uint rank, std::string name, uint score, const NovaColor &color, const NovaVertex &position) {
	this->menuState = menuState;

	// Setup the ranking.
	char buffer[20];
	std::snprintf(buffer, sizeof(buffer), "%d", rank);
	std::string formattedNumber(buffer);
	for (std::string::size_type i = 0; i < formattedNumber.size(); i++) {
		playerRank.push_back(new CharacterDigit(menuState));
		playerRank[i]->init(color);
	}

	// Position of the first digit.
	float x = (position.x - 46);
	float y = position.y;
	float z = position.z;

	// Position the digits then transform and project in advance.
	for (std::string::size_type i = 0; i < formattedNumber.size(); i++) {
		playerRank[i]->moveTo(x, y, z);
		playerRank[i]->transform();
		x += CHARACTER_DIGIT_WIDTH;
	}

	for (std::string::size_type i = 0; i < formattedNumber.size(); i++) {
		int number = std::stoi(std::string(1, formattedNumber[i]));
		playerRank[i]->setVisible(number);
	}

	// Setup the player name.
	uint numCharacters = 0;
	for (char c : name) {
		if (std::isalnum(static_cast<unsigned char>(c))) {
			numCharacters++;
		} else {
			if (std::isblank(static_cast<unsigned char>(c))) {
				numCharacters++;
			} else {
				// Check for any special characters that we accept.
				if (c == '-') {
					numCharacters++;
				}
			}
		}
	}

	// Need to validate that our text count matches (that it is in sync).
	if (numCharacters != name.size()) {
		logWarningMessage("The player name contains characters which cannot be displayed");
	}

	for (uint i = 0; i < numCharacters; i++) {
		playerName.push_back(new CharacterSprite(menuState));
	}

	uint i = 0;
	for (char c : name) {
		if (std::isalnum(static_cast<unsigned char>(c))) {
			if (std::isalpha(c)) {
				c = std::toupper(c);
				int characterIndex = static_cast<int>(c);

				// Convert the ASCII value to our baseline character index (0 == A).
				characterIndex -= 65;
				playerName[i++]->init(menuState->getAlphaCharacter(characterIndex), color);
			} else {
				int numberIndex = static_cast<int>(c);

				// Convert the ASCII value to our baseline number index (0 == 0).
				numberIndex -= 48;
				playerName[i++]->init(menuState->getNumericalCharacter(numberIndex), color);
			}
		} else {
			if (std::isblank(static_cast<unsigned char>(c))) {
				// Skip a space.
				i++;
			} else {
				// Check for any special characters that we accept.
				if (c == '-') {
					playerName[i++]->init(menuState->getSpecialCharacter(DASH_CHARACTER_INDEX), color);
				}
			}
		}
	}

	// Position the first character.
	x = (position.x - 37);
	y = position.y;
	z = position.z;

	// Position the letters then transform and project in advance.
	for (CharacterSprite* character : playerName) {
		if (character->getDefinition()) {
			character->moveTo(x, y, z);
			character->transform();
		}

		// Skip any spaces.
		x += STANDARD_CHARACTER_WIDTH;
	}

	// Setup the player score.
	formattedNumber = formatStringWithCommas(score);

	// Need to check for overflow.
	if (formattedNumber.size() > NUM_SCORE_DIGITS) {
		formattedNumber.resize(NUM_SCORE_DIGITS);
	}

	for (std::string::size_type i = 0; i < formattedNumber.size(); i++) {
		playerScore.push_back(new CharacterDigit(menuState));
		playerScore[i]->init(color);
	}

	// Position the first digit then transform and project in advance.
	x = (position.x + 46);
	y = position.y;
	z = position.z;

	playerScore[0]->moveTo(x, y, z);
	playerScore[0]->transform();

	// We need to display the numbers in reverse order.
	reverse(formattedNumber.begin(), formattedNumber.end());

	for (std::string::size_type i = 0; i < formattedNumber.size(); ++i) {
		if (std::isdigit(formattedNumber[i])) {
			int number = std::stoi(std::string(1, formattedNumber[i]));
			playerScore[i]->setVisible(number);
		} else {
			// Comma separator.
			playerScore[i]->setVisible(COMMA_SEPARATOR_DIGIT);
		}
	}

	// Need to reformat the other digits to take into account comma spacing.
	bool shortSpacing = false;

	for (std::string::size_type i = 1; i < formattedNumber.size(); i++) {
		if (!playerScore[i]->isVisible()) {
			break; // No point continuing.
		}

		if (playerScore[i]->getActiveDigit() == COMMA_SEPARATOR_DIGIT) {
			x -= (CHARACTER_DIGIT_WIDTH - 1);
			shortSpacing = true;
		} else {
			if (shortSpacing) {
				x -= (CHARACTER_DIGIT_WIDTH - 2);

				// Reset.
				shortSpacing = false;
			} else {
				// Normal spacing.
				x -= CHARACTER_DIGIT_WIDTH;
			}
		}

		playerScore[i]->moveTo(x, y, z);
		playerScore[i]->transform();
	}
}

HighScoreEntry::~HighScoreEntry() {
	for (CharacterDigit *digit : playerRank) {
		delete digit;
	}

	playerRank.clear();

	for (CharacterSprite *character : playerName) {
		delete character;
	}

	playerName.clear();

	for (CharacterDigit *digit : playerScore) {
		delete digit;
	}

	playerScore.clear();
}

void HighScoreEntry::draw() {
	glBegin(GL_LINES);
	for (CharacterDigit* digit : playerRank) {
		if (digit->isVisible()) {
			digit->draw();
		} else {
			break; // No point continuing.
		}
	}

	for (CharacterSprite* character : playerName) {
		if (character->getDefinition()) {
			character->draw();
		}
	}

	for (CharacterDigit* digit : playerScore) {
		if (digit->isVisible()) {
			digit->draw();
		} else {
			break; // No point continuing.
		}
	}
	glEnd();
}

