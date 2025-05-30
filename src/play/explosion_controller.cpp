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

ExplosionController::ExplosionController(PlayState* playState) {
	this->playState = playState;
	explosionListHead = NULL;
	playerExplosion = NULL;
}

ExplosionController::~ExplosionController() {
	if (playerExplosion) {
		delete playerExplosion;
		playerExplosion = NULL;
	}

	for (Explosion* explosion : alienExplosions) {
		delete explosion;
	}

	alienExplosions.clear();

	for (Explosion* explosion : missileExplosions) {
		delete explosion;
	}

	missileExplosions.clear();

	for (Explosion* explosion : laserExplosions) {
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

	Yamlish* yamlish = g_worldManager->getYamlish();
	numParticles = yamlish->getInt("explosions.player.particle.density", DEFAULT_NUM_PARTICILES);
	maxVelocity = yamlish->getFloat("explosions.player.particle.velocity", DEFAULT_PARTICILE_VELOCITY);
	duration = yamlish->getFloat("explosions.player.particle.duration", DEFAULT_PARTICILE_DURATION);
	lightEmitting = yamlish->getBool("explosions.player.particle.lightsource", false);
	playerExplosion = new Explosion(playState, numParticles, maxVelocity, duration, lightEmitting);

	numExplosions = yamlish->getInt("explosions.alien.max.instances", DEFAULT_NUM_EXPLOSIONS);
	if (numExplosions > 0) {
		numParticles = yamlish->getInt("explosions.alien.particle.density", DEFAULT_NUM_PARTICILES);
		maxVelocity = yamlish->getFloat("explosions.alien.particle.velocity", DEFAULT_PARTICILE_VELOCITY);
		duration = yamlish->getFloat("explosions.alien.particle.duration", DEFAULT_PARTICILE_DURATION);
		lightEmitting = yamlish->getBool("explosions.alien.particle.lightsource", false);

		for (int i = 0; i < numExplosions; i++) {
			alienExplosions.push_back(new Explosion(playState, numParticles, maxVelocity, duration, lightEmitting));
		}
	}

	numExplosions = yamlish->getInt("explosions.missile.max.instances", DEFAULT_NUM_EXPLOSIONS);
	if (numExplosions > 0) {
		numParticles = yamlish->getInt("explosions.missile.particle.density", DEFAULT_NUM_PARTICILES);
		maxVelocity = yamlish->getFloat("explosions.missile.particle.velocity", DEFAULT_PARTICILE_VELOCITY);
		duration = yamlish->getFloat("explosions.missile.particle.duration", DEFAULT_PARTICILE_DURATION);
		lightEmitting = yamlish->getBool("explosions.missile.particle.lightsource", false);

		for (int i = 0; i < numExplosions; i++) {
			missileExplosions.push_back(new Explosion(playState, numParticles, maxVelocity, duration, lightEmitting));
		}

	}

	numExplosions = yamlish->getInt("explosions.laser.max.instances", DEFAULT_NUM_EXPLOSIONS);
	if (numExplosions > 0) {
		numParticles = yamlish->getInt("explosions.laser.particle.density", DEFAULT_NUM_PARTICILES);
		maxVelocity = yamlish->getFloat("explosions.laser.particle.velocity", DEFAULT_PARTICILE_VELOCITY);
		duration = yamlish->getFloat("explosions.laser.particle.duration", DEFAULT_PARTICILE_DURATION);
		lightEmitting = yamlish->getBool("explosions.missile.particle.lightsource", false);

		for (int i = 0; i < numExplosions; i++) {
			laserExplosions.push_back(new Explosion(playState, numParticles, maxVelocity, duration, lightEmitting));
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
	// Draw active objects.
	Explosion *explosion = explosionListHead;
	while (explosion) {
		if (explosion->isVisible()) {
			explosion->draw();
		}

		explosion = explosion->nextInList;
	}
}

void ExplosionController::createExplosion(Player *player) {
	if (!playerExplosion->isActive()) {
		playerExplosion->moveTo(player->getPositionWCS());

		// This will calculate how much color we would need to reduce (each update) to get to zero at 'max elapsed time'
		playerExplosion->setExplosionColor(player->getExplosionColor());

		g_worldManager->startSound(PLAYER_EXPLODE);

		// This will make the explosion 'active'.
		this->addToList(playerExplosion);
	}
}

void ExplosionController::createExplosion(Alien *alien) {
	Explosion *explosion = NULL;

	// Find an empty slot.
	for (uint i = 0; i < alienExplosions.size(); i++) {
		if (!alienExplosions[i]->isActive()) {
			explosion = alienExplosions[i];
			break;
		}
	}

	if (explosion) {
		explosion->moveTo(alien->getPositionWCS());

		// This will calculate how much color we would need to reduce (each update) to get to zero at 'max elapsed time'
		explosion->setExplosionColor(alien->getExplosionColor());

		g_worldManager->startSound(ENEMY_EXPLODE);

		// This will make the explosion 'active'.
		this->addToList(explosion);
	}
}

void ExplosionController::createExplosion(Missile *missile) {
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

		// This will calculate how much color we would need to reduce (each update) to get to zero at 'max elapsed time'
		explosion->setExplosionColor(missile->getExplosionColor());

		g_worldManager->startSound(MISSILE_EXPLODE);

		// This will make the explosion 'active'.
		this->addToList(explosion);
	}
}

void ExplosionController::createExplosion(Laser *laser) {
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

		// This will calculate how much color we would need to reduce (each update) to get to zero at 'max elapsed time'
		explosion->setExplosionColor(laser->getExplosionColor());

		g_worldManager->startSound(LASER_EXPLODE);

		// This will make the explosion 'active'.
		this->addToList(explosion);
	}
}

void ExplosionController::deactivate(Explosion *explosion) {
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



