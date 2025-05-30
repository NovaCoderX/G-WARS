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

#define MAX_VALUE_LENGTH 12

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

OptionSelector::OptionSelector(MenuState* menuState, const std::string key, int defaultValue,
		std::vector<int> values, const NovaColor& textColor, const NovaVertex& position) : Control(menuState) {
	defaultTextColor = textColor;
	disabledTextColor = defaultTextColor;
	disabledTextColor.rebase(50);
	selectedTextColor = defaultTextColor;
	selectedTextColor.rebase(150);
	currentTextColor = defaultTextColor;
	increasingColor = true;
	this->moveTo(position);

	// Sanity check.
	if (values.size() < 2) {
		fatalError("A selection control needs at least two options\n");
	}

	// Setup the initial value text.
	this->key = key;
	this->values = values;
	this->maxValueIndex = (values.size() - 1);

	// Get the current value from the properties file.
	int currentValue = g_worldManager->getYamlish()->getInt(key, defaultValue);

	// The current value should be contained within the list of values.
	bool found = false;
	for (uint i = 0; i < values.size(); i++) {
		if (values[i] == currentValue) {
			currentValueIndex = i;
			found = true;
			break;
		}
	}

	if (!found) {
		logWarningMessage("The current value is not within the range of possible selection values\n");

		// Default to the first value.
		currentValueIndex = 0;
	}

	this->syncCurrentValue();
}

OptionSelector::~OptionSelector() {
	for (CharacterDigit *digit : displayValue) {
		delete digit;
	}

	displayValue.clear();
}

void OptionSelector::setDisabled(bool disabled) {
	// Base processing.
	Control::setDisabled(disabled);

	if (this->isDisabled()) {
		currentTextColor = disabledTextColor;
	} else {
		currentTextColor = defaultTextColor;
	}

	for (CharacterDigit* digit : displayValue) {
		digit->setSpriteColor(currentTextColor);
	}
}

void OptionSelector::setSelected(bool selected) {
	// Base processing.
	Control::setSelected(selected);

	if (this->isSelected()) {
		currentTextColor = selectedTextColor;
	} else {
		currentTextColor = defaultTextColor;
	}

	for (CharacterDigit* digit : displayValue) {
		digit->setSpriteColor(currentTextColor);
	}
}

void OptionSelector::setInputCaptured(bool inputCaptured) {
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

	for (CharacterDigit* digit : displayValue) {
		digit->setSpriteColor(currentTextColor);
	}
}

void OptionSelector::processInput() {
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

void OptionSelector::update(float elapsedTime) {
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

		for (CharacterDigit* digit : displayValue) {
			digit->setSpriteColor(currentTextColor);
		}
	}
}

void OptionSelector::draw() {
	glBegin(GL_LINES);
	for (CharacterDigit* digit : displayValue) {
		if (digit->isVisible()) {
			digit->draw();
		} else {
			break; // No point continuing.
		}
	}
	glEnd();
}

void OptionSelector::syncCurrentValue() {
	// Sync the displayed value to the current value.
	std::string formattedNumber = formatStringWithCommas(values[currentValueIndex]);

	// Sanity check.
	if (formattedNumber.empty()) {
		logWarningMessage("Option selector contains an invalid value which cannot be displayed\n");
	}

	// Need to check for overflow.
	if (formattedNumber.size() > MAX_VALUE_LENGTH) {
		formattedNumber.resize(MAX_VALUE_LENGTH);
	}

	// Clear out any existing characters.
	for (CharacterDigit *digit : displayValue) {
		delete digit;
	}

	displayValue.clear();

	for (std::string::size_type i = 0; i < formattedNumber.size(); i++) {
		displayValue.push_back(new CharacterDigit(menuState));
		displayValue[i]->init(currentTextColor);
	}

	// Position the first digit.
	NovaVertex position = this->getPositionWCS();
	float x = position.x;
	float y = position.y;
	float z = position.z;

	displayValue[0]->moveTo(x, y, z);
	displayValue[0]->transform();

	for (std::string::size_type i = 0; i < formattedNumber.size(); ++i) {
		if (std::isdigit(formattedNumber[i])) {
			int number = std::stoi(std::string(1, formattedNumber[i]));
			displayValue[i]->setVisible(number);
		} else {
			// Comma separator.
			displayValue[i]->setVisible(COMMA_SEPARATOR_DIGIT);
		}
	}

	// Need to reformat the other digits to take into account comma spacing.
	bool shortSpacing = false;

	for (std::string::size_type i = 1; i < formattedNumber.size(); i++) {
		if (!displayValue[i]->isVisible()) {
			break; // No point continuing.
		}

		if (displayValue[i]->getActiveDigit() == COMMA_SEPARATOR_DIGIT) {
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

		displayValue[i]->moveTo(x, y, z);
		displayValue[i]->transform();
	}
}

void OptionSelector::handleKeyDown(SDL_keysym *keysym) {
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

void OptionSelector::handleKeyUp(SDL_keysym *keysym) {
	// Empty.
}

void OptionSelector::moveCursorLeft() {
	g_worldManager->startSound(UI_CHANGE);

	currentValueIndex--;
	if (currentValueIndex < 0) {
		currentValueIndex = maxValueIndex;
	}

	// Need to regenerate display text based on the current value.
	syncCurrentValue();
}

void OptionSelector::moveCursorRight() {
	g_worldManager->startSound(UI_CHANGE);

	currentValueIndex++;
	if (currentValueIndex > maxValueIndex) {
		currentValueIndex = 0;
	}

	// Need to regenerate text based on the current value.
	syncCurrentValue();
}

void OptionSelector::accept() {
	g_worldManager->startSound(UI_CONFIRM);
	g_worldManager->getYamlish()->setInt(key, values[currentValueIndex]);
	setInputCaptured(false);
}

void OptionSelector::cancel() {
	g_worldManager->startSound(UI_CONFIRM);

	// Get the current value from the properties file.
	int currentValue = g_worldManager->getYamlish()->getInt(key);

	// The current value should be contained within the list of values.
	bool found = false;
	for (uint i = 0; i < values.size(); i++) {
		if (values[i] == currentValue) {
			currentValueIndex = i;
			found = true;
			break;
		}
	}

	if (!found) {
		logWarningMessage("The current value is not within the range of possible selection values\n");

		// Default to the first value.
		currentValueIndex = 0;
	}

	// Need to regenerate display text based on the current value.
	syncCurrentValue();

	setInputCaptured(false);
}
