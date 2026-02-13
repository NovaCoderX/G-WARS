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

// Helper methods.
static bool isBossAlien(AlienType alienType) {
	bool found;

	switch (alienType) {
	case SNAKE_ALIEN:
		found = true;
		break;

	case JELLY_ALIEN:
		found = true;
		break;

	case CRUSHER_ALIEN:
		found = true;
		break;

	default:
		found = false;
	}

	return found;
}

static bool isSpecialAlien(AlienType alienType) {
	bool found;

	switch (alienType) {
	case ROCKET_SHIP_ALIEN:
		found = true;
		break;

	case FLYING_SAUCER_ALIEN:
		found = true;
		break;

	case PLAYER_CLONE_ALIEN:
		found = true;
		break;

	case BLACK_HOLE_ALIEN:
		found = true;
		break;

	case MINI_GATE_ALIEN:
		found = true;
		break;

	case MINI_CRUSHER_ALIEN:
		found = true;
		break;

	default:
		found = false;
	}

	return found;
}

Alien::Alien(PlayState* playState, AlienType alienType) : Sprite(playState) {
	this->playState = playState;
	this->alienType = alienType;

	if (isBossAlien(alienType)) {
		alienCategory = BOSS_ALIEN;
		this->setExplosionSize(MASSIVE_EXPLOSION);
		this->setExplosionSound(BOSS_ALIEN_EXPLODE);
	} else if (isSpecialAlien(alienType)) {
		alienCategory = SPECIAL_ALIEN;
		this->setExplosionSize(LARGE_EXPLOSION);
		this->setExplosionSound(SPECIAL_ALIEN_EXPLODE);
	} else {
		alienCategory = STANDARD_ALIEN;
		this->setExplosionSize(MEDIUM_EXPLOSION);
		this->setExplosionSound(STANDARD_ALIEN_EXPLODE);
	}

	nextInList = priorInList = NULL;
	this->setDefaultColor(NovaColor(255, 255, 255));
	this->setLightEmittingExplosion(true);
}

void Alien::setActive(bool active) {
	// Base processing.
	Sprite::setActive(active);

	if (active) {
		// Reset.
		this->setCurrentColor(this->getDefaultColor());
	} else {
		// We have been destroyed.
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

		if (int_rand(0, 3) == 2) {
			playState->getNuggetController()->spawnNugget(MULTIPLIER_NUGGET, this->getPositionWCS());
		}

		playState->getPlayer()->increaseScore(this);

		// Missile has also been destroyed.
		playState->getMissileController()->deactivate(missile);
	}

	return collision;
}

void Alien::smartBombNotification() {
	// We have been destroyed.
	playState->getAlienController()->deactivate(this);
}
