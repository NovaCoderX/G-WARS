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

#define MIN_INITIAL_VELOCITY 8
#define MAX_INITIAL_VELOCITY 16

RocketShip::RocketShip(PlayState* playState) : Alien(playState, ROCKET_SHIP_ALIEN) {
	this->setSpriteDefinition("rocket_ship");
	highColor = NovaColor(86, 173, 10);
	lowColor = highColor;
	lowColor.rebase(30);
	this->setDefaultColor(lowColor);
	increasingColor = true;
}

void RocketShip::setActive(bool active) {
	// Base processing.
	Alien::setActive(active);

	if (active) {
		// Make some sound.
		g_worldManager->startSound(ENEMY_SPAWN_ROCKET_SHIP);

		// Find out which spawn zone we should be using (based on the players current location).
		if (playState->getPlayAreaController()->isWithinZone(ZoneIndex::LEFT_SPAWN_ZONE, playState->getPlayer())) {
			// Spawn the alien in the right zone.
			this->moveTo(RIGHT_SPAWN_POINT_X, float_rand(-60, 60), 0);
			this->setVelocityFromComponents(-float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY), 0);

			// Rotate the sprite to match the direction of travel.
			this->setFacingTowardsDirection(LEFT);
		} else {
			// Spawn the alien in the left zone.
			this->moveTo(LEFT_SPAWN_POINT_X, float_rand(-60, 60), 0);
			this->setVelocityFromComponents(float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY), 0);

			// Rotate the sprite to match the direction of travel.
			this->setFacingTowardsDirection(RIGHT);
		}

		// Reset.
		increasingColor = true;
	}
}

void RocketShip::update(float elapsedTime) {
	// Apply the velocity.
	this->applyVelocity(elapsedTime);

	// See if we have just moved outside the play area.
	if (this->getFacingTowardsDirection() == LEFT) {
		if (!playState->getPlayAreaController()->isWithinPlayArea(LEFT_BORDER, this)) {
			// We just hit the left border, swap the direction.
			this->reverseHorizontalVelocity();

			// Rotate the sprite to match the direction of travel.
			this->setFacingTowardsDirection(RIGHT);
		}
	} else {
		if (!playState->getPlayAreaController()->isWithinPlayArea(RIGHT_BORDER, this)) {
			// We just hit the right border, swap the direction.
			this->reverseHorizontalVelocity();

			// Rotate the sprite to match the direction of travel.
			this->setFacingTowardsDirection(LEFT);
		}
	}

	// Mark this object visible/invisible for this frame.
	this->calculateVisibility();

	// Animate.
	if (this->isVisible()) {
		// Do some simple color cycling.
		if (increasingColor) {
			if (currentColor.brighten((elapsedTime / 10), highColor)) {
				increasingColor = false;
			}
		} else {
			if (currentColor.fade((elapsedTime / 10), lowColor)) {
				increasingColor = true;
			}
		}
	}
}


