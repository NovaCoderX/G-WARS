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

Alien::Alien(PlayState *playState) : Sprite(playState) {
	this->playState = playState;
	alienType = UNDEFINED_ALIEN;
	nextInList = priorInList = NULL;
}

void Alien::setActive(bool active) {
	if (this->active && (!active)) {
		// We have been destroyed.
		if (this->isVisible()) {
			playState->getExplosionController()->createExplosion(this);
			playState->getNuggetController()->spawnNugget(this);
		}
	}

	// Base processing.
	Sprite::setActive(active);
}

void Alien::checkCollision(Player *player) {
	// See if this alien has just hit the player.
	NovaVertex between = (player->getPositionWCS() - this->getPositionWCS());
	if (between.magnitude() < (player->getBoundingSphere() + this->getBoundingSphere())) {
		// This alien was hit by the player and destroyed.
		playState->getAlienController()->deactivate(this);

		// Optionally destroy the player.
		player->youHit(this);
	}
}

void Alien::checkCollision(Missile *missile) {
	// See if this alien has just hit this missile.
	NovaVertex between = (missile->getPositionWCS() - this->getPositionWCS());
	if (between.magnitude() < (missile->getBoundingSphere() + this->getBoundingSphere())) {
		// This alien was hit by the player's missile.
		playState->getAlienController()->deactivate(this);

		// This alien was destroyed by a player's missile.
		playState->getPlayer()->increaseScore(this);

		// Missile has also been destroyed.
		playState->getMissileController()->deactivate(missile);
	}
}
