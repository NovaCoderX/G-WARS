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

RocketShip::RocketShip(PlayState* playState) : Alien(playState) {
	this->alienType = ROCKET_SHIP;
	this->setSpriteDefinition("rocket_ship");
	this->setSpriteColor(NovaColor(86, 173, 10));
	this->setExplosionColor(this->getSpriteColor());
}

void RocketShip::setActive(bool active) {
	if (active) {
		// Make some sound.
		g_worldManager->startSound(ENEMY_SPAWN_ROCKET_SHIP);
	}

	// Base processing.
	Alien::setActive(active);
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
}


