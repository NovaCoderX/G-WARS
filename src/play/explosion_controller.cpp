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
}

ExplosionController::~ExplosionController() {
	for (Explosion* explosion : smallExplosions) {
		delete explosion;
	}

	smallExplosions.clear();

	for (Explosion* explosion : mediumExplosions) {
		delete explosion;
	}

	mediumExplosions.clear();

	for (Explosion* explosion : largeExplosions) {
		delete explosion;
	}

	largeExplosions.clear();

	for (Explosion* explosion : massiveExplosions) {
		delete explosion;
	}

	massiveExplosions.clear();
}

void ExplosionController::init() {
	int numExplosions;
	int numParticles;
	float maxVelocity;
	float duration;

	Yamlish* yamlish = g_worldManager->getYamlish();

	numExplosions = yamlish->getInt("explosions.small.max.instances", DEFAULT_NUM_EXPLOSIONS);
	if (numExplosions > 0) {
		numParticles = yamlish->getInt("explosions.small.particle.density",	DEFAULT_NUM_PARTICILES);
		maxVelocity = yamlish->getFloat("explosions.small.particle.velocity", DEFAULT_PARTICILE_VELOCITY);
		duration = yamlish->getFloat("explosions.small.particle.duration", DEFAULT_PARTICILE_DURATION);

		for (int i = 0; i < numExplosions; i++) {
			smallExplosions.push_back(new Explosion(playState, numParticles, maxVelocity, duration));
		}
	}

	numExplosions = yamlish->getInt("explosions.medium.max.instances", DEFAULT_NUM_EXPLOSIONS);
	if (numExplosions > 0) {
		numParticles = yamlish->getInt("explosions.medium.particle.density", DEFAULT_NUM_PARTICILES);
		maxVelocity = yamlish->getFloat("explosions.medium.particle.velocity", DEFAULT_PARTICILE_VELOCITY);
		duration = yamlish->getFloat("explosions.medium.particle.duration", DEFAULT_PARTICILE_DURATION);

		for (int i = 0; i < numExplosions; i++) {
			mediumExplosions.push_back(new Explosion(playState, numParticles, maxVelocity, duration));
		}
	}

	numExplosions = yamlish->getInt("explosions.large.max.instances", DEFAULT_NUM_EXPLOSIONS);
	if (numExplosions > 0) {
		numParticles = yamlish->getInt("explosions.large.particle.density", DEFAULT_NUM_PARTICILES);
		maxVelocity = yamlish->getFloat("explosions.large.particle.velocity", DEFAULT_PARTICILE_VELOCITY);
		duration = yamlish->getFloat("explosions.large.particle.duration", DEFAULT_PARTICILE_DURATION);

		for (int i = 0; i < numExplosions; i++) {
			largeExplosions.push_back(new Explosion(playState, numParticles, maxVelocity, duration));
		}
	}

	numExplosions = yamlish->getInt("explosions.massive.max.instances", DEFAULT_NUM_EXPLOSIONS);
	if (numExplosions > 0) {
		numParticles = yamlish->getInt("explosions.massive.particle.density", DEFAULT_NUM_PARTICILES);
		maxVelocity = yamlish->getFloat("explosions.massive.particle.velocity", DEFAULT_PARTICILE_VELOCITY);
		duration = yamlish->getFloat("explosions.massive.particle.duration", DEFAULT_PARTICILE_DURATION);

		for (int i = 0; i < numExplosions; i++) {
			massiveExplosions.push_back(new Explosion(playState, numParticles, maxVelocity, duration));
		}
	}
}

void ExplosionController::update(float elapsedTime) {
	Explosion* explosion = explosionListHead;
	while (explosion) {
		explosion->update(elapsedTime);

		explosion = explosion->nextInList;
	}
}

void ExplosionController::draw() {
	Explosion* explosion = explosionListHead;
	while (explosion) {
		if (explosion->isVisible()) {
			explosion->draw();
		}

		explosion = explosion->nextInList;
	}
}

void ExplosionController::createExplosion(const ExplosiveObject* explosiveObject) {
	Explosion* explosion = this->getNextAvailable(explosiveObject->getExplosionSize());
	if (explosion) {
		explosion->moveTo(explosiveObject->getExplosionOrigin());
		explosion->setColor(explosiveObject->getExplosionColor());
		explosion->setLightEmitting(explosiveObject->isLightEmittingExplosion());
		g_worldManager->startSound(explosiveObject->getExplosionSound());
		this->addToList(explosion);
	}
}

void ExplosionController::deactivate(Explosion* explosion) {
	this->removeFromList(explosion);
}

void ExplosionController::applyLighting(RenderVertex& targetVertex) {
	// Loop through all active explosions and see if our target vertex
	// is within the explosion's bounding sphere (using the explosion origin's CCS).
	// If it is within the bounding sphere then add its color to the vertex light sum.
	Explosion* explosion = explosionListHead;
	while (explosion) {
		if (explosion->isVisible() && explosion->isLightEmitting()) {
			NovaVertex between = (targetVertex - explosion->getPositionCCS());
			if (between.magnitude() < explosion->getBoundingSphere()) {
				targetVertex.lightSum += explosion->getColor();
			}

			if (targetVertex.lightSum.isWhite()) {
				// No point continuing to add light.
				break;
			}
		}

		explosion = explosion->nextInList;
	}
}

Explosion* ExplosionController::getNextAvailable(ExplosionSize size) {
	Explosion* explosion = NULL;

	// Find an empty slot.
	switch (size) {
	case SMALL_EXPLOSION:
		for (uint i = 0; i < smallExplosions.size(); i++) {
			if (!smallExplosions[i]->isActive()) {
				explosion = smallExplosions[i];
				break;
			}
		}
		break;

	case MEDIUM_EXPLOSION:
		for (uint i = 0; i < mediumExplosions.size(); i++) {
			if (!mediumExplosions[i]->isActive()) {
				explosion = mediumExplosions[i];
				break;
			}
		}
		break;

	case LARGE_EXPLOSION:
		for (uint i = 0; i < largeExplosions.size(); i++) {
			if (!largeExplosions[i]->isActive()) {
				explosion = largeExplosions[i];
				break;
			}
		}
		break;

	case MASSIVE_EXPLOSION:
		for (uint i = 0; i < massiveExplosions.size(); i++) {
			if (!massiveExplosions[i]->isActive()) {
				explosion = massiveExplosions[i];
				break;
			}
		}
		break;

	default: // NO_EXPLOSION
		explosion = NULL; // Keep compiler happy.
	}

	return explosion;
}

void ExplosionController::addToList(Explosion* explosion) {
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
		Explosion* last_in_list = explosionListHead;

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

void ExplosionController::removeFromList(Explosion* explosion) {
	Explosion* previousObj = explosion->priorInList;
	Explosion* nextObj = explosion->nextInList;

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

