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

#define NUGGET_DURATION 40

PowerUpNugget::PowerUpNugget(PlayState* playState) : Nugget(playState) {
	nuggetType = POWER_UP;
	this->setSpriteDefinition("alien_nugget");
	highColor = NovaColor(37, 147, 245);
	lowColor = highColor;
	lowColor.rebase(60);
	this->setSpriteColor(highColor);
	increasingColor = false;
	totalElapsedTime = 0;
}

void PowerUpNugget::setActive(bool active) {
	// Base processing.
	Nugget::setActive(active);

	if (active) {
		// Reset.
		this->setSpriteColor(highColor);
		increasingColor = false;
		totalElapsedTime = 0;
	}
}

void PowerUpNugget::update(float elapsedTime) {
	static NovaColor black;

	totalElapsedTime += elapsedTime;

	// See if we need to destroy the nugget (expired).
	if (totalElapsedTime > NUGGET_DURATION) {
		// fade out.
		if (this->spriteColor.fade((elapsedTime / 30), black)) {
			playState->getNuggetController()->deactivate(this);
		}
	} else {
		// Do some simple color cycling.
		if (increasingColor) {
			if (spriteColor.brighten((elapsedTime / 30), highColor)) {
				// Wrap.
				increasingColor = false;
			}
		} else {
			if (spriteColor.fade((elapsedTime / 30), lowColor)) {
				// Wrap.
				increasingColor = true;
			}
		}
	}

	// Mark this object visible/invisible for this frame.
	this->calculateVisibility();
}

