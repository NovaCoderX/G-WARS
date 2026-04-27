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


Alien::Alien(PlayState* playState, AlienCategory alienCategory) : Sprite(playState) {
	this->playState = playState;
	this->alienCategory = alienCategory;
	nextInList = priorInList = NULL;
	active = false;

	// Set up how this object will explode by default.
	this->setExplosionSize(MEDIUM_EXPLOSION);
	this->setExplosionSound(ALIEN_EXPLODE);
	this->setLightEmittingExplosion(true);
}

void Alien::setActive(bool active) {
	if (active) {
		// We have been activated.
		this->active = true;
		this->setVisible(false);
	} else if (this->active) {
		// We have been destroyed.
		this->active = false;
		playState->getExplosionController()->createExplosion(this);
	}
}

bool Alien::checkCollision(Player* player) {
	bool collision = false;

	// See if this alien has just hit the player.
	NovaVertex between = (player->getPositionWCS() - this->getPositionWCS());
	if (between.magnitude() < (player->getBoundingSphere() + this->getBoundingSphere())) {
		// This alien was hit by the player and destroyed.
		collision = true;
		playState->getAlienController()->deactivate(this);

		// Optionally destroy the player.
		player->youHit(this);
	}

	return collision;
}

bool Alien::checkCollision(Missile* missile) {
	bool collision = false;

	// See if this alien has just hit this missile.
	NovaVertex between = (missile->getPositionWCS() - this->getPositionWCS());
	if (between.magnitude() < (missile->getBoundingSphere() + this->getBoundingSphere())) {
		// This alien was destroyed by a player's missile.
		collision = true;
		playState->getAlienController()->deactivate(this);

		// Missile has also been destroyed.
		playState->getMissileController()->deactivate(missile);
	}

	return collision;
}

void Alien::smartBombNotification() {
	// We have been destroyed.
	playState->getAlienController()->deactivate(this);
}

void Alien::miniGateNotification() {
	// We have been destroyed.
	playState->getAlienController()->deactivate(this);
	playState->getPlayer()->increaseScore(this);
	playState->getNuggetController()->spawnNugget(MULTIPLIER_NUGGET, this->getPositionWCS());
}

