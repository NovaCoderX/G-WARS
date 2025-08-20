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

#define DAMAGE_FACTOR 0.05
#define GRAVITY_WELL_RADIUS 100
#define MAXIMUM_GRAVITAIONAL_PULL 4.0

BlackHole::BlackHole(PlayState* playState) : Alien(playState) {
	this->setAlienType(BLACK_HOLE);

	// Create the circle definitions.
	innerRingDefinition = new SpriteDefinition();

	// Create the vertex data.
	int numPoints = 36; // Number of points to calculate along the circle
	float radius = 16; // Radius of the circle

	innerRingDefinition->allocateVertices(numPoints);

	// Loop through each point around the circle
	for (int i = 0; i < numPoints; i++) {
		// Calculate angle in radians for this point
		float angle = (2 * M_PI * i) / numPoints;

		// Calculate the x and y coordinates using cosine and sine
		float x = radius * cos(angle);
		float y = radius * sin(angle);

		innerRingDefinition->setVertex(i, x, y, 0);
	}

	// Create the line data.
	innerRingDefinition->allocateLines(numPoints);
	int startVertex = 0;
	int endVertex = 1;
	for (int i = 0; i < numPoints; i++) {
		innerRingDefinition->setLineVertexMapping(i, startVertex++, endVertex);
		if (endVertex < (numPoints - 1)) {
			endVertex++;
		} else {
			// Close the circle.
			endVertex = 0;
		}
	}

	innerRingDefinition->calculateBoundingSphere();
	this->setSpriteDefinition(innerRingDefinition);
	this->setSpriteColor(NovaColor(255,255,255));
	this->setExplosionSize(MEDIUM_EXPLOSION);
	this->setExplosionColor(this->getSpriteColor());
	this->setNuggetSpawnType(POWER_UP);

	// Outer circle.
	outerRingDefinition = new SpriteDefinition();

	// Create the vertex data.
	numPoints = 36; // Number of points to calculate along the circle
	radius = 16.2; // Radius of the circle

	outerRingDefinition->allocateVertices(numPoints);

	// Loop through each point around the circle
	for (int i = 0; i < numPoints; i++) {
		// Calculate angle in radians for this point
		float angle = (2 * M_PI * i) / numPoints;

		// Calculate the x and y coordinates using cosine and sine
		float x = radius * cos(angle);
		float y = radius * sin(angle);

		outerRingDefinition->setVertex(i, x, y, 0);
	}

	// Create the line data.
	outerRingDefinition->allocateLines(numPoints);
	startVertex = 0;
	endVertex = 1;
	for (int i = 0; i < numPoints; i++) {
		outerRingDefinition->setLineVertexMapping(i, startVertex++, endVertex);
		if (endVertex < (numPoints - 1)) {
			endVertex++;
		} else {
			// Close the circle.
			endVertex = 0;
		}
	}

	outerRingDefinition->calculateBoundingSphere();
	outerRing = new BlackHoleRing(playState, this, outerRingDefinition, NovaColor(255,0,0));
}

BlackHole::~BlackHole() {
	if (innerRingDefinition) {
		delete innerRingDefinition;
		innerRingDefinition = NULL;
	}

	if (outerRingDefinition) {
		delete outerRingDefinition;
		outerRingDefinition = NULL;
	}

	if (outerRing) {
		delete outerRing;
		outerRing = NULL;
	}
}

void BlackHole::setActive(bool active) {
	// Base processing.
	Alien::setActive(active);

	if (active) {
		// Make some sound.
		g_worldManager->startSound(ENEMY_SPAWN_BLACK_HOLE);

		// Reset.
		this->setSpriteColor(NovaColor(255,255,255));
	}

	outerRing->setActive(active);
}

void BlackHole::update(float elapsedTime) {
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
		outerRing->update(elapsedTime);
	}
}

bool BlackHole::checkCollision(Player *player) {
	bool collision = false;

	// See if this alien has just hit the player.
	NovaVertex between = (player->getPositionWCS() - this->getPositionWCS());
	if (between.magnitude() < (playState->getPlayer()->getBoundingSphere() + this->getBoundingSphere())) {
		// This alien was hit by the player.
		collision = true;

		// Optionally destroy the player.
		player->youHit(this);
	}

	return collision;
}

bool BlackHole::checkCollision(Missile *missile) {
	static NovaColor darkGrey = NovaColor(64, 64, 64);
	bool collision = false;

	// This alien was hit by the player's missile.
	NovaVertex between = (missile->getPositionWCS() - this->getPositionWCS());
	if (between.magnitude() < (missile->getBoundingSphere() + this->getBoundingSphere())) {
		// This alien was hit by the player's missile.
		collision = true;

		if (this->spriteColor.fade((DAMAGE_FACTOR), darkGrey)) {
			// This alien was destroyed by the player's missile.
			playState->getAlienController()->deactivate(this);
			playState->getNuggetController()->spawnNugget(this);
			playState->getPlayer()->increaseScore(this);
		}

		// Missile has also been destroyed.
		playState->getMissileController()->deactivate(missile);
	}

	return collision;
}

void BlackHole::draw() {
	// Do base class processing (draw the inner ring).
	Alien::draw();

	// Then drawn the outer ring.
	outerRing->moveTo(this->getPositionWCS());
	outerRing->draw();
}

void BlackHole::applyGravitionalPull(Player *player) {
	static NovaVertex alienPosition;
	static NovaVertex playerPosition;

	alienPosition = this->getPositionWCS();
	playerPosition = player->getPositionWCS();
	float between = (alienPosition - playerPosition).magnitude();
	if (between < GRAVITY_WELL_RADIUS) {
		// Work out the angle from the player to the black hole.
		double alienDirection = calculateDirectionFromVelocityComponents((alienPosition.x - playerPosition.x),
				(alienPosition.y - playerPosition.y));

		// Calculate the gravitation pull based on the distance (the closer, the higher).
		double gravitationalPull = (GRAVITY_WELL_RADIUS - between) / (GRAVITY_WELL_RADIUS / MAXIMUM_GRAVITAIONAL_PULL);

		// Apply the gravitational velocity.
		double radians = (alienDirection + 90) * (M_PI / 180.0);
		double horizontalGravity = (gravitationalPull * cos(radians));
		double verticalGravity = (gravitationalPull * sin(radians));

		if (std::abs(horizontalGravity) < 1e-6) {
			horizontalGravity = 0.0;
		}

		if (std::abs(verticalGravity) < 1e-6) {
			verticalGravity = 0.0;
		}

		player->updateVelocity(horizontalGravity, verticalGravity);
	}
}

/*****************************************************************************
 BlackHoleRing
*****************************************************************************/
BlackHoleRing::BlackHoleRing(PlayState* playState, Alien* parent, SpriteDefinition *definition,
		const NovaColor &color) : AlienComponent(playState, parent) {
	this->setSpriteDefinition(definition);
	increasingColor = true;
	highColor = color;
	lowColor = highColor;
	lowColor.rebase(30);
	this->setSpriteColor(lowColor);
	this->setExplosionSize(MEDIUM_EXPLOSION);
	this->setExplosionColor(this->highColor);
}

void BlackHoleRing::update(float elapsedTime) {
	// Do some simple color cycling.
	if (increasingColor) {
		if (spriteColor.brighten((elapsedTime / 30), highColor)) {
			increasingColor = false;
		}
	} else {
		if (spriteColor.fade((elapsedTime / 30), lowColor)) {
			increasingColor = true;
		}
	}
}
