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

template <typename T>
T clamp(T value, T minVal, T maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

MiniGate::MiniGate(PlayState* playState) : Alien(playState) {
	this->alienType = MINI_GATE;
	head = new Alien(playState);
	head->setSpriteDefinition("mini_gate");
	head->setSpriteColor(NovaColor(252, 152, 3));
	head->setExplosionColor(head->getSpriteColor());

	tail = new Alien(playState);
	tail->setSpriteDefinition("mini_gate");
	tail->setSpriteColor(NovaColor(252, 152, 3));
	tail->setExplosionColor(tail->getSpriteColor());

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
	if (active) {
		// Make some sound.
		g_worldManager->startSound(ENEMY_SPAWN_MINI_GATE);

		// Setup the head and tail platforms.
		NovaVertex headPosition = this->getPositionWCS();
		head->moveTo(headPosition);
		head->setVelocityFromComponents(this->getHorizontalVelocity(), this->getVerticalVelocity());

		// The tail needs to be positioned slightly below and offset from the head.
		NovaVertex tailPosition = this->getPositionWCS();
		tailPosition.x -= GATE_ANGLE;
		tailPosition.y -= GATE_DISTANCE;
		tail->moveTo(tailPosition);
		tail->setVelocityFromComponents(this->getHorizontalVelocity(), this->getVerticalVelocity());

		// Point the head towards the tail.
		double direction = calculateDirectionFromVelocityComponents((tailPosition.x - headPosition.x),
				(tailPosition.y - headPosition.y));
		head->setFacingTowardsDirection(direction);

		// And the tail towards the head.
		direction = calculateDirectionFromVelocityComponents((headPosition.x - tailPosition.x),
				(headPosition.y - tailPosition.y));
		tail->setFacingTowardsDirection(direction);
	} else if (this->isActive()) {
		// We are being destroyed.
		if (head->isVisible() || tail->isVisible()) {
			playState->getExplosionController()->createExplosion(head);
			playState->getExplosionController()->createExplosion(tail);
		}
	}

	// Base processing - Note, we do not want to use the base Alien logic.
	Sprite::setActive(active);
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
		laser->staticVertices[BEGIN].x = headPosition.x;
		laser->staticVertices[BEGIN].y = headPosition.y;
		laser->staticVertices[BEGIN].z = 0;
		laser->laserColor[BEGIN] = currentLaserColor;

		NovaVertex tailPosition = tail->getPositionWCS();
		laser->staticVertices[END].x = tailPosition.x;
		laser->staticVertices[END].y = tailPosition.y;
		laser->staticVertices[END].z = 0;
		laser->laserColor[END] = currentLaserColor;
	}
}

void MiniGate::checkCollision(Player* player) {
	bool collison = false;
	NovaVertex playerPosition = player->getPositionWCS();
	NovaVertex headPosition = head->getPositionWCS();
	NovaVertex tailPosition = tail->getPositionWCS();

	// See if the head has just hit the player.
	NovaVertex between = (playerPosition - headPosition);
	if (between.magnitude() < (player->getBoundingSphere() + head->getBoundingSphere())) {
		collison = true;
	}

	// Also check the tail.
	if (!collison) {
		between = (playerPosition - tailPosition);
		if (between.magnitude() < (player->getBoundingSphere() + tail->getBoundingSphere())) {
			collison = true;
		}
	}

	if (collison) {
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
			// Player went through the gate.
			playState->getPlayer()->increaseScore(this);
			NuggetSpawnMode savedMode = playState->getNuggetController()->getSpawnMode();
			playState->getNuggetController()->setSpawnMode(ALWAYS_SPAWN_NUGGETS);
			playState->getAlienController()->destroyAllLife(); // Including this alien.
			playState->getNuggetController()->setSpawnMode(savedMode);
		}
	}
}

void MiniGate::checkCollision(Missile* missile) {
	bool collison = false;

	// See if the head has just hit this missile.
	NovaVertex between = (missile->getPositionWCS() - head->getPositionWCS());
	if (between.magnitude() < (missile->getBoundingSphere() + head->getBoundingSphere())) {
		collison = true;
	}

	// Also check the tail.
	if (!collison) {
		between = (missile->getPositionWCS() - tail->getPositionWCS());
		if (between.magnitude() < (missile->getBoundingSphere() + tail->getBoundingSphere())) {
			collison = true;
		}
	}

	if (collison) {
		// This alien was hit by the player's missile.
		playState->getAlienController()->deactivate(this);

		// Missile has also been destroyed.
		playState->getMissileController()->deactivate(missile);
	}
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

