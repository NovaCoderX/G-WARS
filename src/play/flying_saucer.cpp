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

#define HIGH_COLOR_INTERVAL 20
#define MINIMUM_CHASE_DISTANCE 10

FlyingSaucer::FlyingSaucer(PlayState* playState) : Alien(playState) {
	this->setAlienType(FLYING_SAUCER);
	this->setSpriteDefinition("flying_saucer");
	highColor = NovaColor(255, 95, 31);
	lowColor = highColor;
	lowColor.rebase(30);
	this->setSpriteColor(lowColor);
	increasingColor = true;
	highColorTimer = 0;
	highColorDisplayed = false;
	this->setExplosionSize(MEDIUM_EXPLOSION);
	this->setExplosionColor(highColor);
	this->setNuggetSpawnType(POWER_UP);
}

void FlyingSaucer::setActive(bool active) {
	// Base processing.
	Alien::setActive(active);

	if (active) {
		// Make some sound.
		g_worldManager->startSound(ENEMY_SPAWN_FLYING_SAUCER);

		this->setSpriteColor(lowColor);
		increasingColor = true;
		highColorTimer = 0;
		highColorDisplayed = false;
	}
}

void FlyingSaucer::update(float elapsedTime) {
	// Check current player position compared to our position (WCS) and move to intercept.
	static NovaVertex playerPosition;
	static NovaVertex alienPosition;

	// Only chase if we are heading towards the player.
	Player *player = playState->getPlayer();
	if ((player->isActive()) && (!player->isShieldActive())) {
		playerPosition = player->getPositionWCS();
		alienPosition = this->getPositionWCS();

		float between = (playerPosition - alienPosition).magnitude();
		if (between > MINIMUM_CHASE_DISTANCE) {
			double playerDirection = calculateDirectionFromVelocityComponents((playerPosition.x - alienPosition.x),
					(playerPosition.y - alienPosition.y));

			// Change our velocity to chase the player.
			this->setVelocityFromDirection(playerDirection, this->getTotalVelocity());
		}
	}

	// Apply the velocity.
	this->applyVelocity(elapsedTime);

	// See if we have just moved outside the play area.
	if (this->getHorizontalVelocity() > 0) {
		if (!playState->getPlayAreaController()->isWithinPlayArea(RIGHT_BORDER, this)) {
			this->reverseHorizontalVelocity();
		}
	} else {
		if (!playState->getPlayAreaController()->isWithinPlayArea(LEFT_BORDER, this)) {
			this->reverseHorizontalVelocity();
		}
	}

	if (this->getVerticalVelocity() > 0) {
		if (!playState->getPlayAreaController()->isWithinPlayArea(TOP_BORDER, this)) {
			this->reverseVerticalVelocity();
		}
	} else {
		if (!playState->getPlayAreaController()->isWithinPlayArea(BOTTOM_BORDER, this)) {
			this->reverseVerticalVelocity();
		}
	}

	// Mark this object visible/invisible for this frame.
	this->calculateVisibility();

	// Animate.
	if (this->isVisible()) {
		if (highColorDisplayed) {
			highColorTimer += elapsedTime;
			if (highColorTimer >= HIGH_COLOR_INTERVAL) {
				// Reset.
				highColorDisplayed = false;
				highColorTimer = 0;
			}
		} else {
			// Do some simple color cycling.
			if (increasingColor) {
				if (spriteColor.brighten((elapsedTime / 30), highColor)) {
					increasingColor = false;

					// Stay at the highest color for a few ticks.
					highColorDisplayed = true;
				}
			} else {
				if (spriteColor.fade((elapsedTime / 30), lowColor)) {
					increasingColor = true;
				}
			}
		}
	}
}

