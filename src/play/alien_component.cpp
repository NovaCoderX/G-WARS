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

AlienComponent::AlienComponent(PlayState* playState) : Sprite(playState) {
	this->playState = playState;
	this->setActiveColor(NovaColor(255, 255, 255));
	this->setInactiveColor(NovaColor(64, 64, 64));
	active = false;

	// Set up how this object will explode by default.
	this->setExplosionSize(NO_EXPLOSION);
	this->setExplosionSound(SILENCE);
	this->setLightEmittingExplosion(true);
}

void AlienComponent::setActive(bool active) {
	if (active) {
		// We have been activated.
		this->active = true;
		this->setVisible(false);
		this->setCurrentColor(activeColor);
	} else if (this->active) {
		// We have been deactivated.
		this->active = false;
		playState->getExplosionController()->createExplosion(this);
		this->setCurrentColor(inactiveColor);
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

