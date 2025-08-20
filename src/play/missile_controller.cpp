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

#define DEFAULT_NUM_PLAYER_MISSILES 100
#define DEFAULT_PLAYER_MISSILE_ADDITIONAL_VELOCITY 8.0
#define DEFAULT_NUM_ALIEN_MISSILES 50
#define DEFAULT_ALIEN_MISSILE_ADDITIONAL_VELOCITY 4.0

MissileController::MissileController(PlayState* playState) {
	this->playState = playState;
	missileListHead = NULL;
	additionalPlayerMissileVelocity = additionalAlienMissileVelocity = 0;
}

MissileController::~MissileController() {
	for (PlayerMissile* missile : playerMissiles) {
		delete missile;
	}

	playerMissiles.clear();

	for (AlienMissile* missile : alienMissiles) {
		delete missile;
	}

	alienMissiles.clear();
}

void MissileController::init() {
	Yamlish* yamlish = g_worldManager->getYamlish();

	int numMissiles = yamlish->getInt("missiles.player.max.instances", DEFAULT_NUM_PLAYER_MISSILES);
	for (int i = 0; i < numMissiles; i++) {
		playerMissiles.push_back(new PlayerMissile(playState));
	}

	additionalPlayerMissileVelocity = yamlish->getFloat("missiles.player.additional.velocity",
			DEFAULT_PLAYER_MISSILE_ADDITIONAL_VELOCITY);

	numMissiles = yamlish->getInt("missiles.alien.max.instances", DEFAULT_NUM_ALIEN_MISSILES);
	for (int i = 0; i < numMissiles; i++) {
		alienMissiles.push_back(new AlienMissile(playState));
	}

	additionalAlienMissileVelocity = yamlish->getFloat("missiles.alien.additional.velocity",
			DEFAULT_ALIEN_MISSILE_ADDITIONAL_VELOCITY);
}

void MissileController::launchMissile(const Player* player) {
	PlayerMissile* firstMissile = NULL;
	PlayerMissile* secondMissile = NULL;
	PlayerMissile* thirdMissile = NULL;

	double direction = player->getFacingTowardsDirection();
	double launchVelocity = player->getTotalVelocity();

	// Find an empty slot.
	for (uint i = 0; i < playerMissiles.size(); i++) {
		if (!playerMissiles[i]->isActive()) {
			firstMissile = playerMissiles[i];
			break;
		}
	}

	if (firstMissile) {
		firstMissile->moveTo(player->getPositionWCS());

		// Set the velocity and direction.
		firstMissile->setVelocityFromDirection(direction, launchVelocity + additionalPlayerMissileVelocity);

		// Rotate the sprite to match the direction of travel.
		firstMissile->setFacingTowardsDirection(direction);

		// Then add it to the processing loop.
		this->addToList(firstMissile);
	}

	if ((firstMissile) && (player->getCurrentPowerUp() == MULTISHOT_POWER_UP)) {
		for (uint i = 0; i < playerMissiles.size(); i++) {
			if (!secondMissile) {
				if (!playerMissiles[i]->isActive()) {
					secondMissile = playerMissiles[i];
				}
			} else {
				if (!playerMissiles[i]->isActive()) {
					thirdMissile = playerMissiles[i];
					break;
				}
			}
		}

		if (secondMissile && thirdMissile) {
			secondMissile->moveTo(player->getPositionWCS());
			thirdMissile->moveTo(player->getPositionWCS());

			float negativeDirection = (direction - 3);
			if (negativeDirection < 0) {
				negativeDirection = (360 + negativeDirection);
			}

			float positiveDirection = (direction + 3);
			if (positiveDirection > 360) {
				positiveDirection = (positiveDirection - 360);
			}

			// Set the velocity and direction.
			secondMissile->setVelocityFromDirection(negativeDirection, launchVelocity + additionalPlayerMissileVelocity);
			thirdMissile->setVelocityFromDirection(positiveDirection, launchVelocity + additionalPlayerMissileVelocity);

			// Rotate the sprites to match the direction of travel.
			secondMissile->setFacingTowardsDirection(negativeDirection);
			thirdMissile->setFacingTowardsDirection(positiveDirection);

			// Then add to the processing loop.
			this->addToList(secondMissile);
			this->addToList(thirdMissile);
		}
	}

	// Make some sound.
	if (secondMissile && thirdMissile) {
		g_worldManager->startSound(FIRE_TRIWAY);
	} else if (firstMissile) {
		if (player->getCurrentPowerUp() == HOMING_MISSILE_POWER_UP) {
			firstMissile->setHomingEnabled(true);
			g_worldManager->startSound(FIRE_HOMING);
		} else {
			if (player->getCurrentPowerUp() == INCREASED_FIRE_RATE_POWER_UP) {
				g_worldManager->startSound(FIRE_HISPEED);
			} else {
				g_worldManager->startSound(FIRE_NORMAL);
			}
		}
	}
}

void MissileController::launchMissile(const NovaVertex& origin, double direction, double exitVelocity) {
	AlienMissile* missile = NULL;

	// Find an empty slot.
	for (uint i = 0; i < alienMissiles.size(); i++) {
		if (!alienMissiles[i]->isActive()) {
			missile = alienMissiles[i];
			break;
		}
	}

	if (missile) {
		missile->moveTo(origin);

		// Set the velocity and direction.
		missile->setVelocityFromDirection(direction, exitVelocity + additionalAlienMissileVelocity);

		// Rotate the sprite to match the direction of travel.
		missile->setFacingTowardsDirection(direction);

		// Then add it to the processing loop.
		this->addToList(missile);

		// Make some sound.
		g_worldManager->startSound(FIRE_NORMAL);
	}
}

void MissileController::update(float elapsedTime) {
	// Update active missiles.
	Missile* missile = missileListHead;
	while (missile) {
		missile->update(elapsedTime);

		missile = missile->nextInList;
	}
}

void MissileController::checkCollisions() {
	Player* player = playState->getPlayer();

	// Check for collisions.
	Missile* missile = missileListHead;
	while (missile) {
		if (missile->isVisible()) {
			if (missile->getMissileType() == PLAYER_MISSILE) {
				Alien* alien = playState->getAlienController()->alienListHead;
				while (alien) {
					if (alien->isVisible()) {
						// See if this alien has just hit this missile.
						if (alien->checkCollision(missile)) {
							// No point continuing because the missile would have been destroyed.
							break;
						}
					}

					alien = alien->nextInList;
				}
			} else if (player->isActive()) {
				// Check to see if the player has just hit this missile.
				player->checkCollision(missile);
			}
		}

		missile = missile->nextInList;
	}
}

void MissileController::draw() {
	// Draw active objects.
	Missile* missile = missileListHead;
	while (missile) {
		if (missile->isVisible()) {
			missile->draw();
		}

		missile = missile->nextInList;
	}
}

void MissileController::deactivate(Missile* missile) {
	this->removeFromList(missile);
}

void MissileController::addToList(Missile* missile) {
	/*
	 ** If no objects have been inserted into the world yet
	 ** then make this object the head of the object list.
	 */
	if (!missileListHead) {
		missileListHead = missile;
		missileListHead->priorInList = NULL;
		missileListHead->nextInList = NULL;
	} else {
		/*
		 ** Otherwise find the last object in the g_world and then
		 ** set the passed object as the new last object in the
		 ** g_world.
		 */
		Missile* last_in_list = missileListHead;

		/*
		 ** Find the last object in the g_world
		 */
		while (last_in_list->nextInList) {
			/*
			 ** See if the passed object has already been inserted
			 */
			if (last_in_list == missile)
				return;

			last_in_list = last_in_list->nextInList;
		}

		/*
		 ** Adjust pointers
		 */
		last_in_list->nextInList = missile;
		missile->priorInList = last_in_list;
		missile->nextInList = NULL;
	}

	// Object is now part of the world (will be drawn).
	missile->setActive(true);
}

void MissileController::removeFromList(Missile* missile) {
	Missile* previousObj = missile->priorInList;
	Missile* nextObj = missile->nextInList;

	// Have to treat list head removal differently.
	if (missile == missileListHead) {
		missileListHead = nextObj;	// Assign new list head.
		if (missileListHead)
			missileListHead->priorInList = NULL;
	} else {
		if (previousObj)
			previousObj->nextInList = nextObj;

		// Set prior reference of the next object if there is one.
		if (nextObj)
			nextObj->priorInList = previousObj;
	}

	// Object is no longer part of the world (will not be drawn).
	missile->setActive(false);
}

