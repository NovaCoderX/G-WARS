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

#define GATE_DISTANCE 18
#define GATE_ANGLE 6
#define MIN_INITIAL_VELOCITY 1
#define MAX_INITIAL_VELOCITY 2

template <typename T>
T clamp(T value, T minVal, T maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

MiniGate::MiniGate(PlayState* playState) : Alien(playState, MINI_GATE_ALIEN) {
	// The alien container doesn't explode.
	this->setExplosionSize(NO_EXPLOSION);

	head = new AlienComponent(playState, this);
	head->setSpriteDefinition("mini_gate");
	head->setDefaultColor(NovaColor(252, 152, 3));
	head->setExplosionSize(MEDIUM_EXPLOSION);
	head->setExplosionSound(SPECIAL_ALIEN_EXPLODE);
	head->setExplosionColor(head->getDefaultColor());


	tail = new AlienComponent(playState, this);
	tail->setSpriteDefinition("mini_gate");
	tail->setDefaultColor(NovaColor(252, 152, 3));
	tail->setExplosionSize(MEDIUM_EXPLOSION);
	tail->setExplosionSound(SPECIAL_ALIEN_EXPLODE);
	tail->setExplosionColor(tail->getDefaultColor());

	laser = new Laser(playState);
	laserHighColor = NovaColor(252, 240, 3);
	laserLowColor = laserHighColor;
	laserLowColor.rebase(30);
	currentLaserColor = laserLowColor;
}

MiniGate::~MiniGate() {
	if (head) {
		delete head;
		head = NULL;
	}

	if (tail) {
		delete tail;
		tail = NULL;
	}

	if (laser) {
		delete laser;
		laser = NULL;
	}
}

void MiniGate::setActive(bool active) {
	static bool verticalSwitch = true;

	// Base processing.
	Alien::setActive(active);

	if (active) {
		// Make some sound.
		g_worldManager->startSound(ENEMY_SPAWN_MINI_GATE);

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
		} else {
			// Spawn the alien in the left zone.
			this->moveTo(LEFT_SPAWN_POINT_X, LEFT_SPAWN_POINT_Y, 0);
			horizontalVelocity = float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY);

			if (verticalSwitch) {
				verticalVelocity = float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY);
			} else {
				verticalVelocity = -float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY);
			}
		}

		verticalSwitch = (!verticalSwitch);

		// Setup the head and tail platforms.
		NovaVertex headPosition = this->getPositionWCS();
		head->moveTo(headPosition);
		head->setVelocityFromComponents(horizontalVelocity, verticalVelocity);

		// The tail needs to be positioned slightly below and offset from the head.
		NovaVertex tailPosition = this->getPositionWCS();
		tailPosition.x -= GATE_ANGLE;
		tailPosition.y -= GATE_DISTANCE;
		tail->moveTo(tailPosition);
		tail->setVelocityFromComponents(horizontalVelocity, verticalVelocity);

		// Point the head towards the tail.
		double direction = calculateDirectionFromVelocityComponents((tailPosition.x - headPosition.x),
				(tailPosition.y - headPosition.y));
		head->setFacingTowardsDirection(direction);

		// And the tail towards the head.
		direction = calculateDirectionFromVelocityComponents((headPosition.x - tailPosition.x),
				(headPosition.y - tailPosition.y));
		tail->setFacingTowardsDirection(direction);
	}

	head->setActive(active);
	tail->setActive(active);
}

void MiniGate::update(float elapsedTime) {
	// Apply the velocity.
	head->applyVelocity(elapsedTime);

	// See if the parent just moved outside the play area.
	if (head->getHorizontalVelocity() > 0) {
		if (!playState->getPlayAreaController()->isWithinPlayArea(RIGHT_BORDER, head)) {
			head->reverseHorizontalVelocity();
		}
	} else {
		if (!playState->getPlayAreaController()->isWithinPlayArea(LEFT_BORDER, head)) {
			head->reverseHorizontalVelocity();
		}
	}

	if (head->getVerticalVelocity() > 0) {
		if (!playState->getPlayAreaController()->isWithinPlayArea(TOP_BORDER, head)) {
			head->reverseVerticalVelocity();
		}
	} else {
		if (!playState->getPlayAreaController()->isWithinPlayArea(BOTTOM_BORDER, head)) {
			head->reverseVerticalVelocity();
		}
	}

	// Apply the velocity.
	tail->applyVelocity(elapsedTime);

	// See if the tail just moved outside the play area.
	if (tail->getHorizontalVelocity() > 0) {
		if (!playState->getPlayAreaController()->isWithinPlayArea(RIGHT_BORDER, tail)) {
			tail->reverseHorizontalVelocity();
		}
	} else {
		if (!playState->getPlayAreaController()->isWithinPlayArea(LEFT_BORDER, tail)) {
			tail->reverseHorizontalVelocity();
		}
	}

	if (tail->getVerticalVelocity() > 0) {
		if (!playState->getPlayAreaController()->isWithinPlayArea(TOP_BORDER, tail)) {
			tail->reverseVerticalVelocity();
		}
	} else {
		if (!playState->getPlayAreaController()->isWithinPlayArea(BOTTOM_BORDER, tail)) {
			tail->reverseVerticalVelocity();
		}
	}

	// Correct velocities to maintain the same distance between the gates.
	NovaVertex headPosition = head->getPositionWCS();
	NovaVertex tailPosition = tail->getPositionWCS();
	NovaVertex delta = (tailPosition - headPosition);
	float currentDistance = delta.magnitude();
	float error = currentDistance - GATE_DISTANCE;

	if (fabs(error) > 0.001f) {
		delta.normalize();

		// Move both platforms equally to maintain the distance.
		NovaVertex correction = delta * (error * 0.5f);
		headPosition += correction;
		tailPosition -= correction;
		head->moveTo(headPosition);
		tail->moveTo(tailPosition);

		// Need to point the platforms towards each other.
		double direction = calculateDirectionFromVelocityComponents((tailPosition.x - headPosition.x),
						(tailPosition.y - headPosition.y));
		head->setFacingTowardsDirection(direction);

		direction = calculateDirectionFromVelocityComponents((headPosition.x - tailPosition.x),
								(headPosition.y - tailPosition.y));
		tail->setFacingTowardsDirection(direction);
	}

	// Mark the platform objects visible/invisible for this frame.
	head->calculateVisibility();
	tail->calculateVisibility();

	// If either platform is visible, then mark the container as visible (so the draw method is called).
	this->setVisible(false);
	if (head->isVisible() || tail->isVisible()) {
		this->setVisible(true);
	}

	if (this->isVisible()) {
		// Do some simple color cycling.
		if (currentLaserColor.brighten((elapsedTime / 2), laserHighColor)) {
			// Reset.
			currentLaserColor = laserLowColor;
		}

		// Need to update the laser start and end positions.
		NovaVertex headPosition = head->getPositionWCS();
		laser->setVertex(LINE_BEGIN, headPosition.x, headPosition.y, 0);
		laser->setColor(LINE_BEGIN, currentLaserColor);

		NovaVertex tailPosition = tail->getPositionWCS();
		laser->setVertex(LINE_END, tailPosition.x, tailPosition.y, 0);
		laser->setColor(LINE_END, currentLaserColor);
	}
}

bool MiniGate::checkCollision(Player* player) {
	bool collision = false;
	NovaVertex playerPosition = player->getPositionWCS();
	NovaVertex headPosition = head->getPositionWCS();
	NovaVertex tailPosition = tail->getPositionWCS();

	// See if the head has just hit the player.
	collision = head->checkCollision(player);

	// Also check the tail.
	if (!collision) {
		collision = tail->checkCollision(player);
	}

	if (collision) {
		// This alien was hit by the player and destroyed.
		playState->getAlienController()->deactivate(this);

		// Optionally destroy the player.
		player->youHit(this);
	} else {
		// The player didn't collide with the platforms, see if the player went through the gate instead.
		NovaVertex ab = (tailPosition - headPosition);
		NovaVertex ap = (playerPosition - headPosition);

		// Find the closest point on the gate to the player.
		float t = ap.dot(ab) / ab.dot(ab);
		t = clamp(t, 0.0f, 1.0f); // Clamp to segment
		NovaVertex closestPoint = (headPosition + ab * t);

		// Compute distance from player to that point.
		NovaVertex difference = (playerPosition - closestPoint);
		float distSq = difference.magnitudeSquared();
		if (distSq <= (player->getBoundingSphere() * player->getBoundingSphere())) {
			// Player went through the gate, destroy all life and spawn nuggets.
			Alien* alien = playState->getAlienController()->alienListHead;
			while (alien) {
				playState->getAlienController()->deactivate(alien);

				if (alien->getAlienCategory() == Alien::STANDARD_ALIEN) {
					playState->getNuggetController()->spawnNugget(MULTIPLIER_NUGGET, alien->getPositionWCS());
				}

				alien = alien->nextInList;
			}

			playState->getPlayer()->increaseScore(this);
		}
	}

	return collision;
}

bool MiniGate::checkCollision(Missile* missile) {
	bool collision = false;

	// See if the head has just hit this missile.
	collision = head->checkCollision(missile);

	// Also check the tail.
	if (!collision) {
		collision = tail->checkCollision(missile);
	}

	if (collision) {
		// This alien was hit by the player's missile.
		playState->getAlienController()->deactivate(this);

		// Missile has also been destroyed.
		playState->getMissileController()->deactivate(missile);
	}

	return collision;
}

void MiniGate::draw() {
	// The laser is visible if either the head or the tail platforms are visible.
	laser->draw();

	if (head->isVisible()) {
		head->draw();
	}

	if (tail->isVisible()) {
		tail->draw();
	}
}

