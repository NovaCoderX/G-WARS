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

#define NUM_CHARACTER_DIGITS 11

CharacterDigit::CharacterDigit(GameState* gameState) {
	this->gameState = gameState;
	visible = false;
	activeDigit = 0;
}

CharacterDigit::~CharacterDigit() {
    for (CharacterSprite* character : characters) {
        delete character;
    }

    characters.clear();
}

void CharacterDigit::setVisible(int activeDigit) {
	if (activeDigit >= NUM_CHARACTER_DIGITS) {
		fatalError("Character index out of range\n");
	}

	// accepts 0 > 10 (inclusive).
	this->activeDigit = activeDigit;
	this->visible = true;
}

void CharacterDigit::init(const NovaColor &color) {
	// Setup the characters.
	for (int i = 0; i < NUM_CHARACTER_DIGITS; i++) {
		characters.push_back(new CharacterSprite(gameState));
	}

	characters[0]->init(g_worldManager->getSpriteDefinition("number_zero"), color);
	characters[1]->init(g_worldManager->getSpriteDefinition("number_one"), color);
	characters[2]->init(g_worldManager->getSpriteDefinition("number_two"), color);
	characters[3]->init(g_worldManager->getSpriteDefinition("number_three"), color);
	characters[4]->init(g_worldManager->getSpriteDefinition("number_four"), color);
	characters[5]->init(g_worldManager->getSpriteDefinition("number_five"), color);
	characters[6]->init(g_worldManager->getSpriteDefinition("number_six"), color);
	characters[7]->init(g_worldManager->getSpriteDefinition("number_seven"), color);
	characters[8]->init(g_worldManager->getSpriteDefinition("number_eight"), color);
	characters[9]->init(g_worldManager->getSpriteDefinition("number_nine"), color);
	characters[COMMA_SEPARATOR_DIGIT]->init(g_worldManager->getSpriteDefinition("comma_separator_digit"), color);
}

void CharacterDigit::setSpriteColor(const NovaColor &color) {
	for (int i = 0; i < NUM_CHARACTER_DIGITS; i++) {
		characters[i]->setSpriteColor(color);
	}
}

void CharacterDigit::moveTo(float x, float y, float z) {
	for (int i = 0; i < NUM_CHARACTER_DIGITS; i++) {
		characters[i]->moveTo(x, y, z);
	}
}

void CharacterDigit::transform() {
	for (int i = 0; i < NUM_CHARACTER_DIGITS; i++) {
		characters[i]->transform();
	}
}

void CharacterDigit::draw() {
	characters[activeDigit]->draw();
}

