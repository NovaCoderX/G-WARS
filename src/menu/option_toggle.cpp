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

OptionToggle::OptionToggle(MenuState* menuState, const std::string key, bool defaultValue,
		const NovaColor& textColor, const NovaVertex& position) : Control(menuState) {
	defaultTextColor = textColor;
	disabledTextColor = defaultTextColor;
	disabledTextColor.rebase(50);
	selectedTextColor = defaultTextColor;
	selectedTextColor.rebase(150);
	currentTextColor = defaultTextColor;
	increasingColor = true;
	this->moveTo(position);

	// Setup the initial value text.
	this->key = key;
	currentValue = g_worldManager->getYamlish()->getBool(key, defaultValue);
	this->syncCurrentValue();
}

OptionToggle::~OptionToggle() {
	for (CharacterSprite *character : displayValue) {
		delete character;
	}

	displayValue.clear();
}

void OptionToggle::setDisabled(bool disabled) {
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

void OptionToggle::setSelected(bool selected) {
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

void OptionToggle::setInputCaptured(bool inputCaptured) {
	// Base processing.
	Control::setInputCaptured(inputCaptured);

	// Reset the text color.
	if (this->isInputCaptured()) {
		// Need to change back to the default for color cycling.
		currentTextColor = defaultTextColor;
	} else {
		// The control will still have focus.
		currentTextColor = selectedTextColor;
	}

	for (CharacterSprite* character : displayValue) {
		character->setSpriteColor(currentTextColor);
	}
}

void OptionToggle::processInput() {
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

void OptionToggle::update(float elapsedTime) {
	if (this->isInputCaptured()) {
		// Do some simple color cycling.
		if (increasingColor) {
			if (currentTextColor.brighten((elapsedTime / 30), selectedTextColor)) {
				increasingColor = false;
			}
		} else {
			if (currentTextColor.fade((elapsedTime / 30), defaultTextColor)) {
				increasingColor = true;
			}
		}

		for (CharacterSprite* character : displayValue) {
			character->setSpriteColor(currentTextColor);
		}
	}
}

void OptionToggle::draw() {
	glBegin(GL_LINES);
	for (CharacterSprite* character : displayValue) {
		if (character->getDefinition()) {
			character->draw();
		}
	}
	glEnd();
}

void OptionToggle::syncCurrentValue() {
	// Sync the displayed value to the current value.
	std::string valueText;

	if (currentValue) {
		valueText = "ENABLED";
	} else {
		valueText = "DISABLED";
	}

	uint numCharacters = 0;
	for (char c : valueText) {
		if (std::isalpha(static_cast<unsigned char>(c))) {
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
	for (char c : valueText) {
		if (std::isalpha(static_cast<unsigned char>(c))) {
			c = std::toupper(c);
			int characterIndex = static_cast<int>(c);

			// Convert the ASCII value to our baseline character index (0 == A).
			characterIndex -= 65;
			displayValue[i++]->init(menuState->getAlphaCharacter(characterIndex), currentTextColor);
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

void OptionToggle::handleKeyDown(SDL_keysym *keysym) {
	switch (keysym->sym) {
	case SDLK_ESCAPE:
		cancel();
		break;
	case SDLK_LEFT:
		moveCursorLeft();
		break;
	case SDLK_RIGHT:
		moveCursorRight();
		break;
	case SDLK_RETURN:
		accept();
		break;
	default:
		break;
	}
}

void OptionToggle::handleKeyUp(SDL_keysym *keysym) {
	// Empty.
}

void OptionToggle::moveCursorLeft() {
	g_worldManager->startSound(UI_CHANGE);

	// Toggle the currently selected value.
	currentValue = (!currentValue);

	// Need to regenerate text based on the current value.
	syncCurrentValue();
}

void OptionToggle::moveCursorRight() {
	g_worldManager->startSound(UI_CHANGE);

	// Toggle the currently selected value.
	currentValue = (!currentValue);

	// Need to regenerate text based on the current value.
	syncCurrentValue();
}

void OptionToggle::accept() {
	g_worldManager->startSound(UI_CONFIRM);
	g_worldManager->getYamlish()->setBool(key, currentValue);
	setInputCaptured(false);
}

void OptionToggle::cancel() {
	g_worldManager->startSound(UI_CONFIRM);
	currentValue = g_worldManager->getYamlish()->getBool(key);
	syncCurrentValue();
	setInputCaptured(false);
}
