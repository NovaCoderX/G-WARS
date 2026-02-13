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


Missile::Missile(PlayState* playState) : Sprite(playState) {
	this->playState = playState;
	missileType = UNDEFINED_MISSILE;
	nextInList = priorInList = NULL;

	// Set up how this object will explode.
	this->setExplosionSize(SMALL_EXPLOSION);
	this->setExplosionSound(MISSILE_EXPLODE);
	this->setLightEmittingExplosion(false);
}

void Missile::setActive(bool active) {
	// Base processing.
	Sprite::setActive(active);

	if (!active) {
		// We have been destroyed.
		playState->getExplosionController()->createExplosion(this);
	}
}

void Missile::update(float elapsedTime) {
	// Apply the velocity.
	this->applyVelocity(elapsedTime);

	// See if we have just moved outside the play area.
	if (!playState->getPlayAreaController()->isWithinPlayArea(this)) {
		// Missile has been destroyed, create an explosion.
		playState->getMissileController()->deactivate(this);
	} else {
		// Mark this object visible/invisible for this frame.
		this->calculateVisibility();
	}
}

