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



Nugget::Nugget(PlayState *playState, NuggetType nuggetType, const NovaColor& color, float duration) : Sprite(playState) {
	this->playState = playState;
	this->nuggetType = nuggetType;
	this->setSpriteDefinition("alien_nugget");
	nextInList = priorInList = NULL;
	highColor = color;
	lowColor = highColor;
	lowColor.rebase(60);
	this->setCurrentColor(highColor);
	increasingColor = false;
	this->duration = duration;
	totalElapsedTime = 0;
	active = false;
}

void Nugget::setActive(bool active) {
	this->active = active;

	if (active) {
		// We have been activated.
		this->setVisible(false);
		this->setCurrentColor(highColor);
		increasingColor = false;
		totalElapsedTime = 0;
	}
}

void Nugget::update(float elapsedTime) {
	static NovaColor black;

	totalElapsedTime += elapsedTime;

	// See if we need to destroy the nugget (expired).
	if (totalElapsedTime > duration) {
		// fade out.
		if (this->currentColor.fade((elapsedTime / 30), black)) {
			playState->getNuggetController()->deactivate(this);
		}
	} else {
		// Do some simple color cycling.
		if (increasingColor) {
			if (currentColor.brighten((elapsedTime / 30), highColor)) {
				// Wrap.
				increasingColor = false;
			}
		} else {
			if (currentColor.fade((elapsedTime / 30), lowColor)) {
				// Wrap.
				increasingColor = true;
			}
		}
	}

	// Mark this object visible/invisible for this frame.
	this->calculateVisibility();
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
