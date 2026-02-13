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

#define SHIELD_DURATION 60
#define SHIELD_FLASH_DELAY 30
#define SHIELD_FLASH_INTERVAL 3

PlayerShield::PlayerShield(PlayState* playState, SpriteDefinition* shieldDefinition) : Sprite(playState) {
	this->setSpriteDefinition(shieldDefinition);
	this->setCurrentColor(NovaColor(255, 255, 255));

	// The shield will remain active until the timer reaches our shield duration time.
	totalElapsedTime = 0;
	lastFlashTime = 0;
}

void PlayerShield::setActive(bool active) {
	// Base processing.
	Sprite::setActive(active);

	if (active) {
		// Shield is visible by default when activated.
		this->setVisible(true);

		// Make some sound.
		g_worldManager->startSound(PLAYER_SHIELD_ON);

		// Reset.
		totalElapsedTime = lastFlashTime = 0;
	} else {
		// Hide the shield.
		this->setVisible(false);
	}
}

void PlayerShield::update(float elapsedTime) {
	totalElapsedTime += elapsedTime;

	// See if we need to turn off the shield.
	if (totalElapsedTime > SHIELD_DURATION) {
		// Turn off the shield.
		this->setActive(false);
	} else {
		// Flash the shield when time is running out.
		if (totalElapsedTime > SHIELD_FLASH_DELAY) {
			// Flash the shield when the shield duration is coming to an end.
			if ((totalElapsedTime - lastFlashTime) >= SHIELD_FLASH_INTERVAL) {
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
		}
	}
}

