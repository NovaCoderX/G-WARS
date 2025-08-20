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

#define DEFAULT_NUM_EXPLOSIONS 10
#define DEFAULT_NUM_PARTICILES 100
#define DEFAULT_PARTICILE_VELOCITY 4.0
#define DEFAULT_PARTICILE_DURATION 10.0

ExplosionController::ExplosionController(PlayState *playState) {
	this->playState = playState;
	explosionListHead = NULL;
	playerExplosion = NULL;
}

ExplosionController::~ExplosionController() {
	if (playerExplosion) {
		delete playerExplosion;
		playerExplosion = NULL;
	}

	for (Explosion *explosion : smallAlienExplosions) {
		delete explosion;
	}

	smallAlienExplosions.clear();

	for (Explosion *explosion : mediumAlienExplosions) {
		delete explosion;
	}

	mediumAlienExplosions.clear();

	for (Explosion *explosion : largeAlienExplosions) {
		delete explosion;
	}

	largeAlienExplosions.clear();

	for (Explosion *explosion : missileExplosions) {
		delete explosion;
	}

	missileExplosions.clear();

	for (Explosion *explosion : laserExplosions) {
		delete explosion;
	}

	laserExplosions.clear();
}

void ExplosionController::init() {
	int numExplosions;
	int numParticles;
	float maxVelocity;
	float duration;
	bool lightEmitting;

	Yamlish *yamlish = g_worldManager->getYamlish();
	numParticles = yamlish->getInt("explosions.player.particle.density",
	DEFAULT_NUM_PARTICILES);
	maxVelocity = yamlish->getFloat("explosions.player.particle.velocity",
	DEFAULT_PARTICILE_VELOCITY);
	duration = yamlish->getFloat("explosions.player.particle.duration",
	DEFAULT_PARTICILE_DURATION);
	lightEmitting = yamlish->getBool("explosions.player.particle.lightsource",
			false);
	playerExplosion = new Explosion(playState, numParticles, maxVelocity,
			duration, lightEmitting);

	numExplosions = yamlish->getInt("explosions.small.alien.max.instances",
	DEFAULT_NUM_EXPLOSIONS);
	if (numExplosions > 0) {
		numParticles = yamlish->getInt(
				"explosions.small.alien.particle.density",
				DEFAULT_NUM_PARTICILES);
		maxVelocity = yamlish->getFloat(
				"explosions.small.alien.particle.velocity",
				DEFAULT_PARTICILE_VELOCITY);
		duration = yamlish->getFloat(
				"explosions.small.alien.particle.duration",
				DEFAULT_PARTICILE_DURATION);
		lightEmitting = yamlish->getBool(
				"explosions.small.alien.particle.lightsource", false);

		for (int i = 0; i < numExplosions; i++) {
			smallAlienExplosions.push_back(
					new Explosion(playState, numParticles, maxVelocity,
							duration, lightEmitting));
		}
	}

	numExplosions = yamlish->getInt("explosions.medium.alien.max.instances",
	DEFAULT_NUM_EXPLOSIONS);
	if (numExplosions > 0) {
		numParticles = yamlish->getInt(
				"explosions.medium.alien.particle.density",
				DEFAULT_NUM_PARTICILES);
		maxVelocity = yamlish->getFloat(
				"explosions.medium.alien.particle.velocity",
				DEFAULT_PARTICILE_VELOCITY);
		duration = yamlish->getFloat(
				"explosions.medium.alien.particle.duration",
				DEFAULT_PARTICILE_DURATION);
		lightEmitting = yamlish->getBool(
				"explosions.medium.alien.particle.lightsource", false);

		for (int i = 0; i < numExplosions; i++) {
			mediumAlienExplosions.push_back(
					new Explosion(playState, numParticles, maxVelocity,
							duration, lightEmitting));
		}
	}

	numExplosions = yamlish->getInt("explosions.large.alien.max.instances",
	DEFAULT_NUM_EXPLOSIONS);
	if (numExplosions > 0) {
		numParticles = yamlish->getInt("explosions.large.alien.particle.density",
		DEFAULT_NUM_PARTICILES);
		maxVelocity = yamlish->getFloat(
				"explosions.large.alien.particle.velocity",
				DEFAULT_PARTICILE_VELOCITY);
		duration = yamlish->getFloat("explosions.large.alien.particle.duration",
		DEFAULT_PARTICILE_DURATION);
		lightEmitting = yamlish->getBool(
				"explosions.large.alien.particle.lightsource", false);

		for (int i = 0; i < numExplosions; i++) {
			largeAlienExplosions.push_back(
					new Explosion(playState, numParticles, maxVelocity,
							duration, lightEmitting));
		}
	}

	numExplosions = yamlish->getInt("explosions.missile.max.instances",
	DEFAULT_NUM_EXPLOSIONS);
	if (numExplosions > 0) {
		numParticles = yamlish->getInt("explosions.missile.particle.density",
		DEFAULT_NUM_PARTICILES);
		maxVelocity = yamlish->getFloat("explosions.missile.particle.velocity",
		DEFAULT_PARTICILE_VELOCITY);
		duration = yamlish->getFloat("explosions.missile.particle.duration",
		DEFAULT_PARTICILE_DURATION);
		lightEmitting = yamlish->getBool(
				"explosions.missile.particle.lightsource", false);

		for (int i = 0; i < numExplosions; i++) {
			missileExplosions.push_back(
					new Explosion(playState, numParticles, maxVelocity,
							duration, lightEmitting));
		}
	}

	numExplosions = yamlish->getInt("explosions.laser.max.instances",
	DEFAULT_NUM_EXPLOSIONS);
	if (numExplosions > 0) {
		numParticles = yamlish->getInt("explosions.laser.particle.density",
		DEFAULT_NUM_PARTICILES);
		maxVelocity = yamlish->getFloat("explosions.laser.particle.velocity",
		DEFAULT_PARTICILE_VELOCITY);
		duration = yamlish->getFloat("explosions.laser.particle.duration",
		DEFAULT_PARTICILE_DURATION);
		lightEmitting = yamlish->getBool(
				"explosions.missile.particle.lightsource", false);

		for (int i = 0; i < numExplosions; i++) {
			laserExplosions.push_back(
					new Explosion(playState, numParticles, maxVelocity,
							duration, lightEmitting));
		}
	}
}

void ExplosionController::update(float elapsedTime) {
	Explosion *explosion = explosionListHead;
	while (explosion) {
		explosion->update(elapsedTime);

		explosion = explosion->nextInList;
	}
}

void ExplosionController::draw() {
	Explosion *explosion = explosionListHead;
	while (explosion) {
		if (explosion->isVisible()) {
			explosion->draw();
		}

		explosion = explosion->nextInList;
	}
}

void ExplosionController::createExplosion(const Player* player) {
	if (!playerExplosion->isActive()) {
		playerExplosion->moveTo(player->getPositionWCS());
		playerExplosion->setExplosionColor(player->getExplosionColor());
		g_worldManager->startSound(PLAYER_EXPLODE);
		this->addToList(playerExplosion);
	}
}

void ExplosionController::createExplosion(const Alien* alien) {
	Explosion *explosion = NULL;

	// Find an empty slot.
	if (alien->getExplosionSize() == SMALL_EXPLOSION) {
		for (uint i = 0; i < smallAlienExplosions.size(); i++) {
			if (!smallAlienExplosions[i]->isActive()) {
				explosion = smallAlienExplosions[i];
				break;
			}
		}
	} else if (alien->getExplosionSize() == MEDIUM_EXPLOSION) {
		for (uint i = 0; i < mediumAlienExplosions.size(); i++) {
			if (!mediumAlienExplosions[i]->isActive()) {
				explosion = mediumAlienExplosions[i];
				break;
			}
		}
	} else if (alien->getExplosionSize() == LARGE_EXPLOSION) {
		for (uint i = 0; i < largeAlienExplosions.size(); i++) {
			if (!largeAlienExplosions[i]->isActive()) {
				explosion = largeAlienExplosions[i];
				break;
			}
		}
	}

	if (explosion) {
		explosion->moveTo(alien->getPositionWCS());
		explosion->setExplosionColor(alien->getExplosionColor());
		g_worldManager->startSound(ENEMY_EXPLODE);
		this->addToList(explosion);
	}
}

void ExplosionController::createExplosion(const AlienComponent* alien) {
	Explosion *explosion = NULL;

	// Find an empty slot.
	if (alien->getExplosionSize() == SMALL_EXPLOSION) {
		for (uint i = 0; i < smallAlienExplosions.size(); i++) {
			if (!smallAlienExplosions[i]->isActive()) {
				explosion = smallAlienExplosions[i];
				break;
			}
		}
	} else if (alien->getExplosionSize() == MEDIUM_EXPLOSION) {
		for (uint i = 0; i < mediumAlienExplosions.size(); i++) {
			if (!mediumAlienExplosions[i]->isActive()) {
				explosion = mediumAlienExplosions[i];
				break;
			}
		}
	} else if (alien->getExplosionSize() == LARGE_EXPLOSION) {
		for (uint i = 0; i < largeAlienExplosions.size(); i++) {
			if (!largeAlienExplosions[i]->isActive()) {
				explosion = largeAlienExplosions[i];
				break;
			}
		}
	}

	if (explosion) {
		explosion->moveTo(alien->getPositionWCS());
		explosion->setExplosionColor(alien->getExplosionColor());
		g_worldManager->startSound(ENEMY_EXPLODE);
		this->addToList(explosion);
	}
}

void ExplosionController::createExplosion(const Missile* missile) {
	Explosion *explosion = NULL;

	// Find an empty slot.
	for (uint i = 0; i < missileExplosions.size(); i++) {
		if (!missileExplosions[i]->isActive()) {
			explosion = missileExplosions[i];
			break;
		}
	}

	if (explosion) {
		explosion->moveTo(missile->getPositionWCS());
		explosion->setExplosionColor(missile->getExplosionColor());
		g_worldManager->startSound(MISSILE_EXPLODE);
		this->addToList(explosion);
	}
}

void ExplosionController::createExplosion(const Laser* laser) {
	Explosion *explosion = NULL;

	// Find an empty slot.
	for (uint i = 0; i < laserExplosions.size(); i++) {
		if (!laserExplosions[i]->isActive()) {
			explosion = laserExplosions[i];
			break;
		}
	}

	if (explosion) {
		explosion->moveTo(laser->getEndPoint());
		explosion->setExplosionColor(laser->getExplosionColor());
		g_worldManager->startSound(LASER_EXPLODE);
		this->addToList(explosion);
	}
}

void ExplosionController::deactivate(Explosion* explosion) {
	this->removeFromList(explosion);
}

void ExplosionController::applyLighting(RenderVertex &targetVertex) {
	// Loop through all active explosions and see if our target vertex
	// is within the explosion's bounding sphere (using the explosion origin's CCS).
	// If it is within the bounding sphere then add its color to the vertex light sum.
	Explosion *explosion = explosionListHead;
	while (explosion) {
		if (explosion->isVisible() && explosion->isLightEmitting()) {
			NovaVertex between = (targetVertex - explosion->getPositionCCS());
			if (between.magnitude() < explosion->getBoundingSphere()) {
				targetVertex.lightSum += explosion->getExplosionColor();
			}

			if (targetVertex.lightSum.isWhite()) {
				// No point continuing to add light.
				break;
			}
		}

		explosion = explosion->nextInList;
	}
}

void ExplosionController::addToList(Explosion *explosion) {
	/*
	 ** If no objects have been inserted into the world yet
	 ** then make this object the head of the object list.
	 */
	if (!explosionListHead) {
		explosionListHead = explosion;
		explosionListHead->priorInList = NULL;
		explosionListHead->nextInList = NULL;
	} else {
		/*
		 ** Otherwise find the last object in the g_world and then
		 ** set the passed object as the new last object in the
		 ** g_world.
		 */
		Explosion *last_in_list = explosionListHead;

		/*
		 ** Find the last object in the g_world
		 */
		while (last_in_list->nextInList) {
			/*
			 ** See if the passed object has already been inserted
			 */
			if (last_in_list == explosion)
				return;

			last_in_list = last_in_list->nextInList;
		}

		/*
		 ** Adjust pointers
		 */
		last_in_list->nextInList = explosion;
		explosion->priorInList = last_in_list;
		explosion->nextInList = NULL;
	}

	// Object is now part of the world (will be drawn).
	explosion->setActive(true);
}

void ExplosionController::removeFromList(Explosion *explosion) {
	Explosion *previousObj = explosion->priorInList;
	Explosion *nextObj = explosion->nextInList;

	// Have to treat list head removal differently.
	if (explosion == explosionListHead) {
		explosionListHead = nextObj;	// Assign new list head.
		if (explosionListHead)
			explosionListHead->priorInList = NULL;
	} else {
		if (previousObj)
			previousObj->nextInList = nextObj;

		// Set prior reference of the next object if there is one.
		if (nextObj)
			nextObj->priorInList = previousObj;
	}

	// Object is no longer part of the world (will not be drawn).
	explosion->setActive(false);
}

