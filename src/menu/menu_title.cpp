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

#define MAX_TITLE_LENGTH 11
#define TITLE_CHARACTER_WIDTH 12

static inline void ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
	}));
}

static inline void rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
	}).base(), s.end());
}

static inline void trim(std::string &s) {
	ltrim(s);
	rtrim(s);
}

static inline void truncate(std::string& s, std::size_t length) {
	if (s.size() > length) {
		s.resize(length);
	}
}

MenuTitle::MenuTitle(MenuState *menuState, std::string text, const NovaColor& textColor, const NovaVertex &position) {
	this->menuState = menuState;
	trim(text);
	truncate(text, MAX_TITLE_LENGTH);

	uint numCharacters = 0;
	for (char c : text) {
		if (std::isalpha(static_cast<unsigned char>(c))) {
			numCharacters++;
		} else {
			if (std::isblank(static_cast<unsigned char>(c))) {
				numCharacters++;
			} else {
				// Check for any special title that we accept.
				if (c == '-') {
					numCharacters++;
				}
			}
		}
	}

	// Need to validate that our text count matches (that it is in sync).
	if (numCharacters != text.size()) {
		logWarningMessage("Title contains characters which cannot be displayed\n");
	}

	for (uint i = 0; i < numCharacters; i++) {
		title.push_back(new CharacterSprite(menuState));
	}

	uint i = 0;
	for (char c : text) {
		if (std::isalpha(static_cast<unsigned char>(c))) {
			c = std::toupper(c);
			int characterIndex = static_cast<int>(c);

			// Convert the ASCII value to our baseline character index (0 == A).
			characterIndex -= 65;
			title[i++]->init(menuState->getAlphaCharacter(characterIndex, LARGE_CHARACTER), textColor);
		} else {
			if (std::isblank(static_cast<unsigned char>(c))) {
				// Skip a space.
				i++;
			} else {
				// Check for any special title that we accept.
				if (c == '-') {
					title[i++]->init(menuState->getSpecialCharacter(DASH_CHARACTER_INDEX, LARGE_CHARACTER), textColor);
				}
			}
		}
	}

	// Position the title.
	float x = position.x;
	float y = position.y;
	float z = position.z;

	// First find the length of the string (fixed width font).
	int length = (title.size() * TITLE_CHARACTER_WIDTH);

	// Calculate the starting point of the first character.
	x = (x - (length / 2));
	x += (TITLE_CHARACTER_WIDTH / 2);

	// Position the letters then transform and project in advance.
	for (CharacterSprite* character : title) {
		if (character->getDefinition()) {
			character->moveTo(x, y, z);
			character->transform();
		}

		// Skip any spaces.
		x += TITLE_CHARACTER_WIDTH;
	}
}

MenuTitle::~MenuTitle() {
	for (CharacterSprite *character : title) {
		delete character;
	}

	title.clear();
}

void MenuTitle::draw() {
	glLineWidth(3.0);
	glBegin(GL_LINES);

	for (CharacterSprite* character : title) {
		if (character->getDefinition()) {
			character->draw();
		}
	}

	glEnd();
	glLineWidth(1.0);
}

