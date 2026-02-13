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

Nugget::Nugget(PlayState *playState, NuggetType nuggetType) : Sprite(playState) {
	this->playState = playState;
	this->nuggetType = nuggetType;
	nextInList = priorInList = NULL;
	defaultColor = NovaColor(255, 255, 255);
}

void Nugget::setActive(bool active) {
	// Base processing.
	Sprite::setActive(active);

	if (active) {
		// Reset.
		this->setCurrentColor(defaultColor);
	}
}

void Nugget::checkCollision(Player *player) {
	// See if this nugget has just been collected by the player.
	NovaVertex between = (player->getPositionWCS() - this->getPositionWCS());
	if (between.magnitude() < (player->getBoundingSphere() + this->getBoundingSphere())) {
		// This nugget was collected.
		playState->getNuggetController()->deactivate(this);

		// Tell the player.
		player->youHit(this);
	}
}
