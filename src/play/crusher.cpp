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

Crusher::Crusher(PlayState* playState) : Alien(playState, CRUSHER_ALIEN) {
	// The alien container doesn't explode.
	this->setExplosionSize(NO_EXPLOSION);

	// Create the two banks.
	leftCrusher = new LeftCrusherPlatform(playState, this);
	rightCrusher = new RightCrusherPlatform(playState, this);
}

Crusher::~Crusher() {
	if (leftCrusher) {
		delete leftCrusher;
		leftCrusher = NULL;
	}

	if (rightCrusher) {
		delete rightCrusher;
		rightCrusher = NULL;
	}
}

void Crusher::setActive(bool active) {
	// Base processing.
	Alien::setActive(active);

	if (active) {
		// Make some sound.
		g_worldManager->startSound(ENEMY_SPAWN_BOSS);

		// Set initial position, direction and velocity of the new alien.
		this->moveTo(0, 0, 0);
		this->setVelocityFromComponents(0, 0);
	}

	leftCrusher->setActive(active);
	rightCrusher->setActive(active);
}

void Crusher::update(float elapsedTime) {
	leftCrusher->update(elapsedTime);
	rightCrusher->update(elapsedTime);

	this->setVisible(false);
	if (leftCrusher->isVisible() || rightCrusher->isVisible()) {
		this->setVisible(true);
	}

	// See if we are now dead (both platforms are inactive and have finished retreating).
	if ((!leftCrusher->isActive()) && (!rightCrusher->isActive())) {
		if ((!leftCrusher->isRetreating()) && (!rightCrusher->isRetreating())) {
			playState->getAlienController()->deactivate(this);
			playState->getPlayer()->increaseScore(this);
		}
	}
}

bool Crusher::checkCollision(Player *player) {
	bool collision = false;

	collision = leftCrusher->checkCollision(player);
	if (!collision) {
		collision = rightCrusher->checkCollision(player);
	}

	if (collision) {
		// This alien was hit by the player, optionally destroy the player.
		player->youHit(this);
	}

	return collision;
}

bool Crusher::checkCollision(Missile *missile) {
	bool collision = false;

	collision = leftCrusher->checkCollision(missile);
	if (!collision) {
		collision = rightCrusher->checkCollision(missile);
	}

	if (collision) {
		if ((!leftCrusher->isActive()) && (!rightCrusher->isActive())) {
			leftCrusher->setRetreating(true);
			rightCrusher->setRetreating(true);
		}

		// Missile has been destroyed.
		playState->getMissileController()->deactivate(missile);
	}

	return collision;
}

void Crusher::smartBombNotification() {
	/*if (leftCrusher->isActive()) {
		leftCrusher->smartBombNotification();
	} else if (rightCrusher->isActive()) {
		rightCrusher->smartBombNotification();
	}

	if ((!leftCrusher->isActive()) && (!rightCrusher->isActive())) {
		leftCrusher->setRetreating(true);
		rightCrusher->setRetreating(true);
	}*/
	playState->getAlienController()->deactivate(this);
}

void Crusher::draw() {
	leftCrusher->draw();
	rightCrusher->draw();
}



