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

#define MINIMUM_CHASE_DISTANCE 10
#define MAXIMUM_CHASE_ANGLE 10

ChaseAlien::ChaseAlien(PlayState* playState) : Alien(playState) {
	// Empty.
}

void ChaseAlien::update(float elapsedTime) {
	// Only chase if we are heading towards the player.
	Player *player = playState->getPlayer();
	if ((player->isActive()) && (!player->isShieldActive())) {
		NovaVertex playerPosition = player->getPositionWCS();
		NovaVertex alienPosition = this->getPositionWCS();

		float between = (playerPosition - alienPosition).magnitude();
		if (between > MINIMUM_CHASE_DISTANCE) {
			double playerDirection = calculateDirectionFromVelocityComponents((playerPosition.x - alienPosition.x),
					(playerPosition.y - alienPosition.y));
			double movementDirection = calculateDirectionFromVelocityComponents(this->getHorizontalVelocity(),
					this->getVerticalVelocity());

			float diff = fabs(movementDirection - playerDirection);
			if (diff < MAXIMUM_CHASE_ANGLE) {
				// Change our velocity to chase the player.
				this->setVelocityFromDirection(playerDirection, this->getTotalVelocity());
			}
		}
	}

	// Apply the velocity.
	this->applyVelocity(elapsedTime);

	// See if we have just moved outside the play area.
	if (this->getHorizontalVelocity() > 0) {
		if (!playState->getPlayAreaController()->isWithinPlayArea(RIGHT_BORDER, this)) {
			// We just hit the right border.
			this->reverseHorizontalVelocity();
		}
	} else {
		if (!playState->getPlayAreaController()->isWithinPlayArea(LEFT_BORDER, this)) {
			// We just hit the left border.
			this->reverseHorizontalVelocity();
		}
	}

	if (this->getVerticalVelocity() > 0) {
		if (!playState->getPlayAreaController()->isWithinPlayArea(TOP_BORDER, this)) {
			// We just hit the top border, make vertical velocity negative.
			this->reverseVerticalVelocity();
		}
	} else {
		if (!playState->getPlayAreaController()->isWithinPlayArea(BOTTOM_BORDER, this)) {
			// We just hit the bottom border, make vertical velocity positive.
			this->reverseVerticalVelocity();
		}
	}

	// Mark this object visible/invisible for this frame.
	this->calculateVisibility();
}



