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

#define INITIAL_FIRE_DELAY 30
#define FIRE_INTERVAL 20
#define READY_TO_FIRE_DELAY 3
#define MAXIMUM_TARGET_ANGLE 30
#define MUZZLE_VERTEX_ANCHOR 9

AttackTank::AttackTank(PlayState* playState) : Alien(playState) {
	this->setAlienType(ATTACK_TANK);
	defaultColorTurret = NovaColor(169, 169, 169);
	defaultColorBase = defaultColorTurret;
	defaultColorBase.rebase(80);
	readyToFireColorTurret = NovaColor(255, 201, 4);
	readyToFireColorBase = readyToFireColorTurret;
	readyToFireColorBase.rebase(80);
	readyToFire = false;
	totalElapsedTime = lastFireTime = readyToFireTime = 0;

	this->setSpriteDefinition("attack_tank_base");
	this->setSpriteColor(defaultColorBase);
	this->setExplosionColor(readyToFireColorTurret);

	gunTurret = new Sprite(playState);
	gunTurret->setSpriteDefinition("attack_tank_turret");
	gunTurret->setSpriteColor(defaultColorTurret);
}

AttackTank::~AttackTank() {
	if (gunTurret) {
		delete gunTurret;
		gunTurret = NULL;
	}
}

void AttackTank::setActive(bool active) {
	// Base processing.
	Alien::setActive(active);

	if (active) {
		// Rotate the sprite towards the direction of travel.
		double movementDirection = calculateDirectionFromVelocityComponents(this->getHorizontalVelocity(), this->getVerticalVelocity());
		this->setFacingTowardsDirection(movementDirection);

		// Reset.
		readyToFire = false;
		this->setSpriteColor(defaultColorBase);
		gunTurret->setSpriteColor(defaultColorTurret);
		totalElapsedTime = lastFireTime = readyToFireTime = 0;
	}
}

void AttackTank::update(float elapsedTime) {
	totalElapsedTime += elapsedTime;

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
		double movementDirection = calculateDirectionFromVelocityComponents(this->getHorizontalVelocity(),
				this->getVerticalVelocity());
		this->setFacingTowardsDirection(movementDirection);
	}

	// Mark this object visible/invisible for this frame.
	this->calculateVisibility();

	if (this->visible) {
		gunTurret->moveTo(this->getPositionWCS());
		Player *player = playState->getPlayer();
		if (player->isActive() && (!player->isShieldActive())) {
			NovaVertex playerPosition = player->getPositionWCS();
			NovaVertex alienPosition = gunTurret->getPositionWCS();
			double playerDirection = calculateDirectionFromVelocityComponents((playerPosition.x - alienPosition.x),
					(playerPosition.y - alienPosition.y));

			// Rotate the turret to point towards the player.
			gunTurret->setFacingTowardsDirection(playerDirection);

			if (readyToFire) {
				// This is just so that we stay highlighted for a couple of seconds before firing.
				if ((totalElapsedTime - readyToFireTime) >= READY_TO_FIRE_DELAY) {
					// Need to check to see if we are traveling roughly in the correct direction to fire.
					if (fabs(this->getFacingTowardsDirection() - playerDirection) < MAXIMUM_TARGET_ANGLE) {
						playState->getMissileController()->launchMissile(gunTurret->getAnchorPointWCS(MUZZLE_VERTEX_ANCHOR),
															playerDirection, this->getTotalVelocity());

						// Reset.
						readyToFire = false;
						this->setSpriteColor(defaultColorBase);
						gunTurret->setSpriteColor(defaultColorTurret);

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
						this->setSpriteColor(readyToFireColorBase);
						gunTurret->setSpriteColor(readyToFireColorTurret);
					}
				}
			}
		} else {
			// Rotate the turret to point towards the direction of travel.
			gunTurret->setFacingTowardsDirection(this->getFacingTowardsDirection());
		}
	}
}

void AttackTank::draw() {
	// Do base processing to draw the base.
	Sprite::draw();

	// If the base is visible, then so is the gun turret.
	gunTurret->draw();
}

