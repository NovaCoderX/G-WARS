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

#define MINIMUM_DISTANCE_TO_PLAYER 60
#define MINIMUM_FIRE_DISTANCE 20
#define MAXIMUM_FIRE_DISTANCE 100
#define AUTOFIRE_INTERVAL 12

PlayerClone::PlayerClone(PlayState* playState) : Alien(playState) {
	this->setAlienType(PLAYER_CLONE);
	this->setSpriteDefinition("player");
	this->setSpriteColor(NovaColor(255, 255, 255));
	this->setExplosionColor(this->getSpriteColor());
	this->setExplosionSize(MEDIUM_EXPLOSION);
	this->setNuggetSpawnType(POWER_UP);
	autofireTimer = 0;

	// Create the shield definition.
	shieldDefinition = new SpriteDefinition();

	// Create the vertex data.
	int numPoints = 72; // Number of points to calculate along the circle
	float radius = 6.0; // Radius of the circle

	shieldDefinition->allocateVertices(numPoints);

	// Loop through each point around the circle
	for (int i = 0; i < numPoints; i++) {
		// Calculate angle in radians for this point
		float angle = (2 * M_PI * i) / numPoints;

		// Calculate the x and y coordinates using cosine and sine
		float x = radius * cos(angle);
		float y = radius * sin(angle);

		shieldDefinition->setVertex(i, x, y, 0);
	}

	// Create the line data.
	shieldDefinition->allocateLines(numPoints);
	int startVertex = 0;
	int endVertex = 1;
	for (int i = 0; i < numPoints; i++) {
		shieldDefinition->setLineVertexMapping(i, startVertex++, endVertex);

		if (endVertex < (numPoints - 1)) {
			endVertex++;
		} else {
			// Close the circle.
			endVertex = 0;
		}
	}

	shieldDefinition->calculateBoundingSphere();

	// Create the shield.
	alienShield = new AlienShield(playState, shieldDefinition);
}

PlayerClone::~PlayerClone() {
	if (alienShield) {
		delete alienShield;
		alienShield = NULL;
	}

	if (shieldDefinition) {
		delete shieldDefinition;
		shieldDefinition = NULL;
	}
}

void PlayerClone::setActive(bool active) {
	// Base processing.
	Alien::setActive(active);

	if (active) {
		// Make some sound.
		g_worldManager->startSound(ENEMY_SPAWN_PLAYER_CLONE);
	}
}

void PlayerClone::update(float elapsedTime) {
	static NovaVertex playerPosition;
	double playerDirection = 0;

	autofireTimer += elapsedTime;

	Player *player = playState->getPlayer();
	if (player->isActive()) {
		NovaVertex alienPosition = this->getPositionWCS();
		playerPosition = player->getPositionWCS();

		// Change velocity to travel towards the active player.
		playerDirection = calculateDirectionFromVelocityComponents((playerPosition.x - alienPosition.x),
				(playerPosition.y - alienPosition.y));
		this->setVelocityFromDirection(playerDirection, this->getTotalVelocity());

		// Rotate the sprite to match the direction of travel.
		this->setFacingTowardsDirection(playerDirection);

		// Cannot get too close to the player.
		float between = (playerPosition - alienPosition).magnitude();
		if (between > MINIMUM_DISTANCE_TO_PLAYER) {
			this->applyVelocity(elapsedTime);
		}
	}

	// Mark this object visible/invisible for this frame.
	this->calculateVisibility();

	if (this->isVisible()) {
		if (player->isShieldActive()) {
			// If player's shield is active then activate our shield.
			alienShield->setActive(true);

			if (player->isShieldVisible()) {
				alienShield->setVisible(true);
			} else {
				alienShield->setVisible(false);
			}
		} else {
			// Otherwise hide our shield if it is currently visible.
			if (alienShield->isActive()) {
				alienShield->setActive(false);
				alienShield->setVisible(false);
			}

			// See if we are now ready to fire.
			if (player->isActive()) {
				NovaVertex alienPosition = this->getPositionWCS();
				float between = (playerPosition - alienPosition).magnitude();

				// We need to check that we are within firing range.
				if ((between > MINIMUM_FIRE_DISTANCE) && (between < MAXIMUM_FIRE_DISTANCE)) {
					if (autofireTimer >= AUTOFIRE_INTERVAL) {
						playState->getMissileController()->launchMissile(this->getPositionWCS(),
								this->getFacingTowardsDirection(), this->getTotalVelocity());

						// Reset.
						autofireTimer = 0;
					}
				}
			}
		}
	}
}

bool PlayerClone::checkCollision(Player *player) {
	bool collision = false;

	// See if this alien has just hit the player.
	NovaVertex between = (player->getPositionWCS() - this->getPositionWCS());
	if (between.magnitude() < (playState->getPlayer()->getBoundingSphere() + this->getBoundingSphere())) {
		// This alien was hit by the player.
		collision = true;

		if (!alienShield->isActive()) {
			playState->getAlienController()->deactivate(this);
		}

		// Optionally destroy the player.
		player->youHit(this);
	}

	return collision;
}

bool PlayerClone::checkCollision(Missile *missile) {
	bool collision = false;

	// See if this alien has just hit this missile.
	NovaVertex between = (missile->getPositionWCS() - this->getPositionWCS());
	if (between.magnitude() < (missile->getBoundingSphere() + this->getBoundingSphere())) {
		// This alien was hit by the player's missile.
		collision = true;

		if (!alienShield->isActive()) {
			// This alien was destroyed by a player's missile.
			playState->getAlienController()->deactivate(this);
			playState->getNuggetController()->spawnNugget(this);
			playState->getPlayer()->increaseScore(this);
		}

		// Missile has been destroyed.
		playState->getMissileController()->deactivate(missile);
	}

	return collision;
}

void PlayerClone::draw() {
	// Do base class processing.
	Sprite::draw();

	// Also draw the shield if it is visible.
	if (alienShield->isVisible()) {
		// Keep the shield position in sync with the player.
		alienShield->moveTo(this->getPositionWCS());
		alienShield->draw();
	}
}


