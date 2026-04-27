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

#define MIN_INITIAL_VELOCITY 6
#define MAX_INITIAL_VELOCITY 12

MiniCrusher::MiniCrusher(PlayState* playState) : Alien(playState, SPECIAL_ALIEN) {
	// The alien container doesn't explode.
	this->setExplosionSize(NO_EXPLOSION);

	highColor = NovaColor(3, 202, 255);
	lowColor = highColor;
	lowColor.rebase(30);
	increasingColor = true;
	nuggetSpawned = false;

	top = new AlienComponent(playState);
	top->setSpriteDefinition("mini_crusher");
	top->setActiveColor(highColor);
	top->setExplosionSize(MEDIUM_EXPLOSION);
	top->setExplosionSound(SPECIAL_ALIEN_EXPLODE);

	bottom = new AlienComponent(playState);
	bottom->setSpriteDefinition("mini_crusher");
	bottom->setActiveColor(highColor);
	bottom->setExplosionSize(MEDIUM_EXPLOSION);
	bottom->setExplosionSound(SPECIAL_ALIEN_EXPLODE);
}

MiniCrusher::~MiniCrusher() {
	if (top) {
		delete top;
		top = NULL;
	}

	if (bottom) {
		delete bottom;
		bottom = NULL;
	}
}

void MiniCrusher::setActive(bool active) {
	static bool verticalSwitch = true;

	// Base processing.
	Alien::setActive(active);

	if (active) {
		// Make some sound.
		g_worldManager->startSound(ENEMY_SPAWN_MINI_CRUSHER);

		// Set initial position, direction and velocity of the new alien.
		this->moveTo(0, 0, 0);
		this->setVelocityFromComponents(0, 0);

		// Set up the top and bottom platforms.
		if (verticalSwitch) {
			top->moveTo(-VERTICAL_BORDER_POSITION, HORIZONTAL_BORDER_POSITION, 0);
			bottom->moveTo(VERTICAL_BORDER_POSITION, -HORIZONTAL_BORDER_POSITION, 0);
		} else {
			top->moveTo(VERTICAL_BORDER_POSITION, HORIZONTAL_BORDER_POSITION, 0);
			bottom->moveTo(-VERTICAL_BORDER_POSITION, -HORIZONTAL_BORDER_POSITION, 0);
		}

		// Switch.
		verticalSwitch = (!verticalSwitch);

		NovaVertex topPosition = top->getPositionWCS();
		NovaVertex bottomPosition = bottom->getPositionWCS();
		float initialVelocity = float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY);

		double direction = calculateDirectionFromVelocityComponents((bottomPosition.x - topPosition.x),
							(bottomPosition.y - topPosition.y));
		top->setVelocityFromDirection(direction, initialVelocity);
		top->setFacingTowardsDirection(direction);

		direction = calculateDirectionFromVelocityComponents((topPosition.x - bottomPosition.x),
							(topPosition.y - bottomPosition.y));
		bottom->setVelocityFromDirection(direction, initialVelocity);
		bottom->setFacingTowardsDirection(direction);

		// Reset.
		increasingColor = true;
		nuggetSpawned = false;
	}

	// Set up the crushers.
	top->setActive(active);
	bottom->setActive(active);
}

void MiniCrusher::update(float elapsedTime) {
	if (top->isActive()) {
		// Apply the velocity.
		top->applyVelocity(elapsedTime);

		if (top->getVerticalVelocity() > 0) {
			if (!playState->getPlayAreaController()->isWithinPlayArea(TOP_BORDER, top)) {
				top->reverseVelocity();

				// Rotate the sprite towards the direction of travel.
				double movementDirection = calculateDirectionFromVelocityComponents(top->getHorizontalVelocity(), top->getVerticalVelocity());
				top->setFacingTowardsDirection(movementDirection);
			}
		} else {
			if (!playState->getPlayAreaController()->isWithinPlayArea(BOTTOM_BORDER, top)) {
				top->reverseVelocity();

				// Rotate the sprite towards the direction of travel.
				double movementDirection = calculateDirectionFromVelocityComponents(top->getHorizontalVelocity(), top->getVerticalVelocity());
				top->setFacingTowardsDirection(movementDirection);
			}
		}

		// Mark the crusher object visible/invisible for this frame.
		top->calculateVisibility();
	} else {
		top->setVisible(false);
	}

	if (bottom->isActive()) {
		// Apply the velocity.
		bottom->applyVelocity(elapsedTime);

		// See if the bottom just moved outside the play area.
		if (bottom->getVerticalVelocity() > 0) {
			if (!playState->getPlayAreaController()->isWithinPlayArea(TOP_BORDER, bottom)) {
				bottom->reverseVelocity();

				// Rotate the sprite towards the direction of travel.
				double movementDirection = calculateDirectionFromVelocityComponents(bottom->getHorizontalVelocity(), bottom->getVerticalVelocity());
				bottom->setFacingTowardsDirection(movementDirection);
			}
		} else {
			if (!playState->getPlayAreaController()->isWithinPlayArea(BOTTOM_BORDER, bottom)) {
				bottom->reverseVelocity();

				// Rotate the sprite towards the direction of travel.
				double movementDirection = calculateDirectionFromVelocityComponents(bottom->getHorizontalVelocity(), bottom->getVerticalVelocity());
				bottom->setFacingTowardsDirection(movementDirection);
			}
		}

		// Mark the crusher object visible/invisible for this frame.
		bottom->calculateVisibility();
	} else {
		bottom->setVisible(false);
	}

	// If either crusher is visible, then mark the container as visible (so the draw method is called).
	this->setVisible(false);
	if (top->isVisible() || bottom->isVisible()) {
		this->setVisible(true);
	}

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

		top->setCurrentColor(this->getCurrentColor());
		bottom->setCurrentColor(this->getCurrentColor());
	}
}

bool MiniCrusher::checkCollision(Player* player) {
	bool collision = false;

	if (top->isActive()) {
		// See if the top has just hit the player.
		collision = top->checkCollision(player);
		if (collision) {
			top->setActive(false);
		}
	}

	// Also check the bottom.
	if (!collision) {
		if (bottom->isActive()) {
			collision = bottom->checkCollision(player);
			if (collision) {
				bottom->setActive(false);
			}
		}
	}

	if (collision) {
		if ((!top->isActive()) && (!bottom->isActive())) {
			// This alien was hit by the player and destroyed.
			playState->getAlienController()->deactivate(this);
		}

		// Optionally destroy the player.
		player->youHit(this);
	}

	return collision;
}

bool MiniCrusher::checkCollision(Missile* missile) {
	bool collision = false;

	if (top->isActive()) {
		// See if the top has just hit this missile.
		collision = top->checkCollision(missile);
		if (collision) {
			top->setActive(false);

			if (!nuggetSpawned) {
				playState->getNuggetController()->spawnNugget(POWER_UP_NUGGET, top->getPositionWCS());
				nuggetSpawned = true;
			}
		}
	}

	// Also check the bottom.
	if (!collision) {
		if (bottom->isActive()) {
			collision = bottom->checkCollision(missile);
			if (collision) {
				bottom->setActive(false);

				if (!nuggetSpawned) {
					playState->getNuggetController()->spawnNugget(POWER_UP_NUGGET, bottom->getPositionWCS());
					nuggetSpawned = true;
				}
			}
		}
	}

	if (collision) {
		if ((!top->isActive()) && (!bottom->isActive())) {
			// This alien was hit by the player's missile and destroyed.
			playState->getAlienController()->deactivate(this);
			playState->getPlayer()->increaseScore(this);
		}

		// Missile has also been destroyed.
		playState->getMissileController()->deactivate(missile);
	}

	return collision;
}

void MiniCrusher::miniGateNotification() {
	// We have been destroyed.
	playState->getAlienController()->deactivate(this);
	playState->getPlayer()->increaseScore(this);

	if (top->isActive()) {
		top->setActive(false);

		if (!nuggetSpawned) {
			playState->getNuggetController()->spawnNugget(MULTIPLIER_NUGGET, top->getPositionWCS());
			nuggetSpawned = true;
		}
	}

	if (bottom->isActive()) {
		bottom->setActive(false);

		if (!nuggetSpawned) {
			playState->getNuggetController()->spawnNugget(MULTIPLIER_NUGGET, bottom->getPositionWCS());
			nuggetSpawned = true;
		}
	}
}

void MiniCrusher::draw() {
	if (top->isVisible()) {
		top->draw();
	}

	if (bottom->isVisible()) {
		bottom->draw();
	}
}

