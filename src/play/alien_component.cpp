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

AlienComponent::AlienComponent(PlayState* playState, Sprite* parent) : Sprite(playState) {
	this->playState = playState;
	this->parent = parent;
	defaultColor = NovaColor(255, 255, 255);
	disabledColor = NovaColor(64, 64, 64);
	explosionColor = NovaColor(255, 255, 255);
	exploded = false;

	// Set up how this object will explode.
	this->setExplosionSize(NO_EXPLOSION);
	this->setExplosionSound(SILENCE);
	this->setLightEmittingExplosion(true);
}

void AlienComponent::setActive(bool active) {
	// Base processing.
	Sprite::setActive(active);

	if (active) {
		// Reset.
		this->setCurrentColor(this->getDefaultColor());
		exploded = false;
	} else {
		this->setCurrentColor(disabledColor);

		// We only want to explode when we are first deactivated.
		if (!exploded) {
			playState->getExplosionController()->createExplosion(this);
			exploded = true;
		}
	}
}

bool AlienComponent::checkCollision(Player* player) {
	bool collision = false;

	// See if this alien component has just hit the player.
	NovaVertex between = (player->getPositionWCS() - this->getPositionWCS());
	if (between.magnitude() < (player->getBoundingSphere() + this->getBoundingSphere())) {
		collision = true;
	}

	return collision;
}

bool AlienComponent::checkCollision(Missile* missile) {
	bool collision = false;

	// See if this alien component has just hit this missile.
	NovaVertex between = (missile->getPositionWCS() - this->getPositionWCS());
	if (between.magnitude() < (missile->getBoundingSphere() + this->getBoundingSphere())) {
		collision = true;
	}

	return collision;
}

