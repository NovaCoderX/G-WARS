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

#define FLASH_DURATION 30
#define FLASH_INTERVAL 3

PlayerShield::PlayerShield(PlayState* playState, SpriteDefinition* shieldDefinition) : Sprite(playState) {
	this->playState = playState;
	this->setSpriteDefinition(shieldDefinition);

	// The shield will remain active until the timer reaches our shield duration time.
	totalElapsedTime = 0;
	lastFlashTime = 0;
	flashDelay = 0;
	active = false;
}

void PlayerShield::setActive(bool active) {
	if (active) {
		// We have been activated.
		this->active = true;
		this->setVisible(true);

		// Make some sound.
		g_worldManager->startSound(PLAYER_SHIELD_ON);

		// Calculate the flash delay based on the shield's duration.
		flashDelay = (playState->getPlayer()->getShieldDuration() - FLASH_DURATION);
		if (flashDelay < FLASH_DURATION) {
			flashDelay = FLASH_DURATION;
		}

		// Reset.
		totalElapsedTime = lastFlashTime = 0;
	} else {
		// We have been deactivated.
		this->active = false;
		this->setVisible(false);
	}
}

void PlayerShield::update(float elapsedTime) {
	totalElapsedTime += elapsedTime;

	// Flash the shield when time is running out.
	if (totalElapsedTime > flashDelay) {
		if ((totalElapsedTime - lastFlashTime) >= FLASH_INTERVAL) {
			// Switch.
			this->setVisible(!this->isVisible());

			if (this->isVisible()) {
				g_worldManager->startSound(PLAYER_SHIELD_ON);
			} else {
				g_worldManager->startSound(PLAYER_SHIELD_OFF);
			}

			// Store.
			lastFlashTime = totalElapsedTime;
		}

		if (totalElapsedTime > (flashDelay + FLASH_DURATION)) {
			// The shield has expired.
			this->setActive(false);
		}
	}
}

void PlayerShield::draw() {
	// Shield color should always match the color of the player.
	if (this->currentColor != playState->getPlayer()->getCurrentColor()) {
		this->currentColor = playState->getPlayer()->getCurrentColor();
	}

	// Do base class processing.
	Sprite::draw();
}

