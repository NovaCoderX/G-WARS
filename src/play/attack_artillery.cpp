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
#define MAXIMUM_CHASE_ANGLE 60
#define INITIAL_FIRE_DELAY 30
#define FIRE_INTERVAL 20
#define READY_TO_FIRE_DELAY 6
#define MAXIMUM_TARGET_ANGLE 30
#define MUZZLE_VERTEX_ANCHOR 14

AttackArtillery::AttackArtillery(PlayState* playState) : Alien(playState) {
	this->setAlienType(ATTACK_ARTILLERY);
	this->setSpriteDefinition("attack_artillery");
	defaultColor = NovaColor(169, 169, 169);
	readyToFireColor = NovaColor(255, 201, 4);
	this->setSpriteColor(defaultColor);
	this->setExplosionColor(readyToFireColor);
	readyToFire = false;
	totalElapsedTime = lastFireTime = readyToFireTime = 0;
}

void AttackArtillery::setActive(bool active) {
	// Base processing.
	Alien::setActive(active);

	if (active) {
		// Rotate the sprite towards the direction of travel.
		double movementDirection = calculateDirectionFromVelocityComponents(this->getHorizontalVelocity(), this->getVerticalVelocity());
		this->setFacingTowardsDirection(movementDirection);

		// Reset.
		readyToFire = false;
		this->setSpriteColor(defaultColor);
		totalElapsedTime = lastFireTime = readyToFireTime = 0;
	}
}

void AttackArtillery::update(float elapsedTime) {
	totalElapsedTime += elapsedTime;

	// Only chase if we are heading towards the player.
	Player *player = playState->getPlayer();
	if (readyToFire && (player->isActive()) && (!player->isShieldActive())) {
		NovaVertex playerPosition = player->getPositionWCS();
		NovaVertex alienPosition = this->getPositionWCS();

		float between = (playerPosition - alienPosition).magnitude();
		if (between > MINIMUM_CHASE_DISTANCE) {
			double playerDirection = calculateDirectionFromVelocityComponents((playerPosition.x - alienPosition.x),
					(playerPosition.y - alienPosition.y));
			double movementDirection = calculateDirectionFromVelocityComponents(this->getHorizontalVelocity(), this->getVerticalVelocity());

			float diff = fabs(movementDirection - playerDirection);
			if (diff < MAXIMUM_CHASE_ANGLE) {
				// Change our velocity to chase the player.
				this->setVelocityFromDirection(playerDirection, this->getTotalVelocity());
				this->setFacingTowardsDirection(playerDirection);
			}
		}
	}

	// Apply the velocity.
	this->applyVelocity(elapsedTime);

	// See if we have just moved outside the play area.
	bool borderViolated = false;
	if (this->getHorizontalVelocity() > 0) {
		if (!playState->getPlayAreaController()->isWithinPlayArea(RIGHT_BORDER, this)) {
			borderViolated = true;
			this->reverseHorizontalVelocity();
		}
	} else {
		if (!playState->getPlayAreaController()->isWithinPlayArea(LEFT_BORDER, this)) {
			borderViolated = true;
			this->reverseHorizontalVelocity();
		}
	}

	if (this->getVerticalVelocity() > 0) {
		if (!playState->getPlayAreaController()->isWithinPlayArea(TOP_BORDER, this)) {
			borderViolated = true;
			this->reverseVerticalVelocity();
		}
	} else {
		if (!playState->getPlayAreaController()->isWithinPlayArea(BOTTOM_BORDER, this)) {
			borderViolated = true;
			this->reverseVerticalVelocity();
		}
	}

	if (borderViolated) {
		// Rotate the sprite towards the direction of travel.
		double movementDirection = calculateDirectionFromVelocityComponents(this->getHorizontalVelocity(), this->getVerticalVelocity());
		this->setFacingTowardsDirection(movementDirection);
	}

	// Mark this object visible/invisible for this frame.
	this->calculateVisibility();

	if (this->visible && player->isActive() && (!player->isShieldActive())) {
		NovaVertex playerPosition = player->getPositionWCS();
		NovaVertex alienPosition = this->getPositionWCS();
		double playerDirection = calculateDirectionFromVelocityComponents((playerPosition.x - alienPosition.x),
				(playerPosition.y - alienPosition.y));

		if (readyToFire) {
			// This is just so that we stay highlighted for a couple of seconds before firing.
			if ((totalElapsedTime - readyToFireTime) >= READY_TO_FIRE_DELAY) {
				// Need to check to see if we are traveling roughly in the correct direction to fire.
				if (fabs(this->getFacingTowardsDirection() - playerDirection) < MAXIMUM_TARGET_ANGLE) {
					playState->getMissileController()->launchMissile(this->getAnchorPointWCS(MUZZLE_VERTEX_ANCHOR),
							this->getFacingTowardsDirection(), this->getTotalVelocity());

					// Reset.
					readyToFire = false;
					this->setSpriteColor(defaultColor);

					// Store the last time that we fired.
					lastFireTime = totalElapsedTime;
				}
			}
		} else {
			// See if we are now ready to fire.
			if (totalElapsedTime > INITIAL_FIRE_DELAY) {
				if ((totalElapsedTime - lastFireTime) >= FIRE_INTERVAL) {
					// Ready to fire another missile, change the color to let the player know.
					readyToFire = true;
					readyToFireTime = totalElapsedTime;
					this->setSpriteColor(readyToFireColor);
				}
			}
		}
	}
}

