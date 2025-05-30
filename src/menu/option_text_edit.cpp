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

#define MAX_TEXT_VALUE_LENGTH 12
#define INPUT_PANEL_WIDTH 52
#define INPUT_PANEL_HEIGHT 6

static inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
	}));
}

static inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
	}).base(), s.end());
}

static inline void trim(std::string& s) {
	ltrim(s);
	rtrim(s);
}

static inline void truncate(std::string& s, std::size_t length) {
	if (s.size() > length) {
		s.resize(length);
	}
}

OptionTextEdit::OptionTextEdit(MenuState* menuState, const std::string key, std::string defaultValue,
		const NovaColor& textColor, const NovaVertex& position) : Control(menuState) {
	defaultTextColor = textColor;
	disabledTextColor = defaultTextColor;
	disabledTextColor.rebase(50);
	selectedTextColor = defaultTextColor;
	selectedTextColor.rebase(150);
	editTextColor = NovaColor(0, 0, 0);
	currentTextColor = defaultTextColor;
	this->moveTo(position);

	// Setup the initial value text.
	this->key = key;
	this->defaultValue = defaultValue;

	currentValue = g_worldManager->getYamlish()->get(key);
	if (currentValue.empty()) {
		currentValue = defaultValue;
	}

	trim(currentValue);
	truncate(currentValue, MAX_TEXT_VALUE_LENGTH);
	this->syncCurrentValue();

	// Setup the input panel background.
	float x = (position.x + 23);
	float y = position.y;
	float z = position.z;

	panelColor = NovaColor(255, 255, 255);
	NovaColor panelBorderColor = DEFAULT_TEXT_COLOR;
	panelBorderColor.rebase(20);
	inputPanel = new MenuPanel(menuState, INPUT_PANEL_WIDTH, INPUT_PANEL_HEIGHT,
			panelColor, panelBorderColor, NovaVertex(x, y, z));

	// Create the input cursor.
	cursor = new CharacterSprite(menuState);
	cursor->init(menuState->getSpecialCharacter(CURSOR_CHARACTER_INDEX), editTextColor);
}

OptionTextEdit::~OptionTextEdit() {
	for (CharacterSprite *character : displayValue) {
		delete character;
	}

	displayValue.clear();

	if (inputPanel) {
		delete inputPanel;
		inputPanel = NULL;
	}

	if (cursor) {
		delete cursor;
		cursor = NULL;
	}
}

void OptionTextEdit::setDisabled(bool disabled) {
	// Base processing.
	Control::setDisabled(disabled);

	if (this->isDisabled()) {
		currentTextColor = disabledTextColor;
	} else {
		currentTextColor = defaultTextColor;
	}

	for (CharacterSprite* character : displayValue) {
		character->setSpriteColor(currentTextColor);
	}
}

void OptionTextEdit::setSelected(bool selected) {
	// Base processing.
	Control::setSelected(selected);

	if (this->isSelected()) {
		currentTextColor = selectedTextColor;
	} else {
		currentTextColor = defaultTextColor;
	}

	for (CharacterSprite* character : displayValue) {
		character->setSpriteColor(currentTextColor);
	}
}

void OptionTextEdit::setInputCaptured(bool inputCaptured) {
	// Base processing.
	Control::setInputCaptured(inputCaptured);

	// Reset the text color.
	if (this->isInputCaptured()) {
		// Need to change back to the default for color cycling.
		currentTextColor = editTextColor;
	} else {
		// The control will still have focus.
		currentTextColor = selectedTextColor;
	}

	for (CharacterSprite* character : displayValue) {
		character->setSpriteColor(currentTextColor);
	}

	// Draw or hide the edit box and cursor.
	if (this->isInputCaptured()) {
		inputPanel->setVisible(true);

		// Reset.
		currentCursorColor = editTextColor;
		cursor->setSpriteColor(currentCursorColor);
		increasingCursorColor = true;

		// The initial position of the cursor is based on the length of the current text value.
		cursorPosition = displayValue.size();
		this->syncCursorPosition();
	} else {
		inputPanel->setVisible(false);
	}
}

void OptionTextEdit::processInput() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_JOYAXISMOTION:
			if (event.jaxis.axis == 0) {
				// Horizontal movement.
				if (event.jaxis.value < 0) {
					moveCursorLeft();
				} else if (event.jaxis.value > 0) {
					moveCursorRight();
				}
			}
			break;
		case SDL_JOYBUTTONDOWN:
			if (event.jbutton.button == 0) {
				accept();
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

void OptionTextEdit::update(float elapsedTime) {
	// Flash the cursor by changing the color (black/white).
	if (this->isInputCaptured()) {
		if (increasingCursorColor) {
			if (currentCursorColor.brighten((elapsedTime / 3), panelColor)) {
				increasingCursorColor = false;
			}
		} else {
			if (currentCursorColor.fade((elapsedTime / 3), editTextColor)) {
				increasingCursorColor = true;
			}
		}

		cursor->setSpriteColor(currentCursorColor);
	}
}

void OptionTextEdit::draw() {
	if (inputPanel->isVisible()) {
		inputPanel->draw();

		glBegin(GL_LINES);
		cursor->draw();
		glEnd();
	}

	glBegin(GL_LINES);
	for (CharacterSprite* character : displayValue) {
		if (character->getDefinition()) {
			character->draw();
		}
	}
	glEnd();
}

void OptionTextEdit::syncCurrentValue() {
	// Sync the displayed value to the current value.
	uint numCharacters = 0;
	for (char c : currentValue) {
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
	if (numCharacters != currentValue.size()) {
		logWarningMessage("Text control contains characters which cannot be displayed\n");
	}

	// Clear out any existing characters.
	for (CharacterSprite *character : displayValue) {
		delete character;
	}

	displayValue.clear();

	// Create the new characters.
	for (uint i = 0; i < numCharacters; i++) {
		displayValue.push_back(new CharacterSprite(menuState));
	}

	uint i = 0;
	for (char c : currentValue) {
		if (std::isalnum(static_cast<unsigned char>(c))) {
			if (std::isalpha(c)) {
				c = std::toupper(c);
				int characterIndex = static_cast<int>(c);

				// Convert the ASCII value to our baseline character index (0 == A).
				characterIndex -= 65;
				displayValue[i++]->init(menuState->getAlphaCharacter(characterIndex), currentTextColor);
			} else {
				int numberIndex = static_cast<int>(c);

				// Convert the ASCII value to our baseline number index (0 == 0).
				numberIndex -= 48;
				displayValue[i++]->init(menuState->getNumericalCharacter(numberIndex), currentTextColor);
			}
		} else {
			if (std::isblank(static_cast<unsigned char>(c))) {
				// Skip a space.
				i++;
			} else {
				// Check for any special characters that we accept.
				if (c == '-') {
					displayValue[i++]->init(menuState->getSpecialCharacter(DASH_CHARACTER_INDEX), currentTextColor);
				}
			}
		}
	}

	// Position the first character.
	NovaVertex position = this->getPositionWCS();
	float x = position.x;
	float y = position.y;
	float z = position.z;

	// Position the letters then transform and project in advance.
	for (CharacterSprite* character : displayValue) {
		if (character->getDefinition()) {
			character->moveTo(x, y, z);
			character->transform();
		}

		// Skip any spaces.
		x += STANDARD_CHARACTER_WIDTH;
	}
}

void OptionTextEdit::syncCursorPosition() {
	// Sync the displayed cursor position to the current position.
	NovaVertex position = this->getPositionWCS();
	float x = position.x + (cursorPosition * STANDARD_CHARACTER_WIDTH);
	float y = position.y;
	float z = position.z;

	cursor->moveTo(x, y, z);
	cursor->transform();
}

void OptionTextEdit::handleKeyDown(SDL_keysym *keysym) {
	switch (keysym->sym) {
	case SDLK_ESCAPE:
		cancel();
		break;
	case SDLK_RETURN:
		accept();
		break;
	case SDLK_LEFT:
		moveCursorLeft();
		break;
	case SDLK_RIGHT:
		moveCursorRight();
		break;
	case SDLK_BACKSPACE:
		if (cursorPosition > 0) {
			// First move the cursor back and then delete.
			moveCursorLeft();
			deleteCharacter();
		}
		break;
	case SDLK_DELETE:
		deleteCharacter();
		break;
	default:
		// Process characters.
		SDLKey keyCode = keysym->sym;
		bool supportedCharacter = false;

		if ((keyCode >= SDLK_a) && (keyCode <= SDLK_z)) {
			supportedCharacter = true;
		}

		if ((keyCode >= SDLK_0) && (keyCode <= SDLK_9)) {
			supportedCharacter = true;
		}

		if (keyCode == SDLK_MINUS) {
			supportedCharacter = true;
		}

		if (keyCode == SDLK_SPACE) {
			supportedCharacter = true;
		}

		if (supportedCharacter) {
			// We can only add supported (displayable) characters.
			insertCharacter(static_cast<char>(keyCode));
		}
		break;
	}
}

void OptionTextEdit::handleKeyUp(SDL_keysym *keysym) {
	// Empty.
}

void OptionTextEdit::moveCursorLeft() {
	if (cursorPosition > 0) {
		cursorPosition--;
		syncCursorPosition();
	}
}

void OptionTextEdit::moveCursorRight() {
	if (cursorPosition < displayValue.size()) {
		cursorPosition++;
		syncCursorPosition();
	}
}

void OptionTextEdit::deleteCharacter() {
	if ((cursorPosition < displayValue.size()) && (displayValue.size() > 0)) {
		currentValue.erase(cursorPosition, 1);
		syncCurrentValue();
	}
}

void OptionTextEdit::insertCharacter(char character) {
	if (currentValue.size() < MAX_TEXT_VALUE_LENGTH) {
		if (std::isalpha(character)) {
			character = std::toupper(character);
		}

		currentValue.insert(cursorPosition, 1, character);
		syncCurrentValue();
		moveCursorRight();
	}
}

void OptionTextEdit::accept() {
	g_worldManager->startSound(UI_CONFIRM);

	if (currentValue.empty()) {
		currentValue = defaultValue;
		syncCurrentValue();
	}

	trim(currentValue);
	truncate(currentValue, MAX_TEXT_VALUE_LENGTH);
	g_worldManager->getYamlish()->set(key, currentValue);
	setInputCaptured(false);
}

void OptionTextEdit::cancel() {
	g_worldManager->startSound(UI_CONFIRM);
	currentValue = g_worldManager->getYamlish()->get(key);
	syncCurrentValue();
	setInputCaptured(false);
}
