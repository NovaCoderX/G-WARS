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

#define HIGH_COLOR_INTERVAL 10
#define MIN_CHASE_DISTANCE 10
#define MIN_INITIAL_VELOCITY 1.3
#define MAX_INITIAL_VELOCITY 2.6

FlyingSaucer::FlyingSaucer(PlayState* playState) : Alien(playState, FLYING_SAUCER_ALIEN) {
	this->setSpriteDefinition("flying_saucer");
	highColor = NovaColor(255, 95, 31);
	lowColor = highColor;
	lowColor.rebase(10);
	this->setDefaultColor(highColor);
	increasingColor = false;
	highColorTimer = 0;
	highColorDisplayed = false;
}

void FlyingSaucer::setActive(bool active) {
	static bool verticalSwitch = true;

	// Base processing.
	Alien::setActive(active);

	if (active) {
		// Make some sound.
		g_worldManager->startSound(ENEMY_SPAWN_FLYING_SAUCER);

		// Find out which spawn zone we should be using (based on the players current location).
		if (playState->getPlayAreaController()->isWithinZone(ZoneIndex::LEFT_SPAWN_ZONE, playState->getPlayer())) {
			// Spawn the alien in the right zone.
			this->moveTo(RIGHT_SPAWN_POINT_X, RIGHT_SPAWN_POINT_Y, 0);
			horizontalVelocity = -float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY);

			if (verticalSwitch) {
				verticalVelocity = float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY);
			} else {
				verticalVelocity = -float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY);
			}

			this->setVelocityFromComponents(horizontalVelocity, verticalVelocity);
		} else {
			// Spawn the alien in the left zone.
			this->moveTo(LEFT_SPAWN_POINT_X, LEFT_SPAWN_POINT_Y, 0);
			horizontalVelocity = float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY);

			if (verticalSwitch) {
				verticalVelocity = float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY);
			} else {
				verticalVelocity = -float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY);
			}

			this->setVelocityFromComponents(horizontalVelocity, verticalVelocity);
		}

		verticalSwitch = (!verticalSwitch);

		// Reset.
		increasingColor = false;
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
		if (between > MIN_CHASE_DISTANCE) {
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
				if (currentColor.brighten((elapsedTime / 60), highColor)) {
					increasingColor = false;

					// Stay at the highest color for a few ticks.
					highColorDisplayed = true;
				}
			} else {
				if (currentColor.fade((elapsedTime / 30), lowColor)) {
					increasingColor = true;
				}
			}
		}
	}
}

bool FlyingSaucer::checkCollision(Missile* missile) {
	bool collision = false;

	// See if this alien has just hit this missile.
	NovaVertex between = (missile->getPositionWCS() - this->getPositionWCS());
	if (between.magnitude() < (missile->getBoundingSphere() + this->getBoundingSphere())) {
		// This alien was destroyed by a player's missile.
		collision = true;
		playState->getAlienController()->deactivate(this);
		playState->getNuggetController()->spawnNugget(POWER_UP_NUGGET, this->getPositionWCS());
		playState->getPlayer()->increaseScore(this);

		// Missile has also been destroyed.
		playState->getMissileController()->deactivate(missile);
	}

	return collision;
}

