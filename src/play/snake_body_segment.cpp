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

#define DAMAGE_FACTOR 0.05

SnakeBodySegment::SnakeBodySegment(PlayState* playState, const NovaColor &highColor) : SnakeSegment(playState) {
	parent = NULL;
	disabled = false;
	this->setSpriteDefinition("snake_segment");
	this->highColor = highColor;
	this->lowColor = highColor;
	this->lowColor.rebase(80);
	this->setSpriteColor(highColor);
	this->setExplosionColor(this->getSpriteColor());
}

void SnakeBodySegment::setActive(bool active) {
	if (active) {
		// Reset.
		this->setDisabled(false);
		this->setSpriteColor(highColor);
	}

	// Base processing.
	SnakeSegment::setActive(active);
}

void SnakeBodySegment::update(float elapsedTime) {
	static NovaVertex alienPosition;
	static NovaVertex parentPosition;

	// Always move towards the parent object.
	alienPosition = this->getPositionWCS();
	parentPosition = parent->getPositionWCS();

	double parentDirection = calculateDirectionFromVelocityComponents((parentPosition.x - alienPosition.x),
			(parentPosition.y - alienPosition.y));
	double horzVelocity = parentPosition.x - alienPosition.x;
	double vertVelocity = parentPosition.y - alienPosition.y;
	double velocity = sqrt(horzVelocity * horzVelocity + vertVelocity * vertVelocity);
	this->setVelocityFromDirection(parentDirection, velocity);
	this->setFacingTowardsDirection(parentDirection);

	// Apply the velocity.
	this->applyVelocity(elapsedTime);

	// Mark this object visible/invisible for this frame.
	this->calculateVisibility();
}

void SnakeBodySegment::youHit(Player *player) {
	// This segment was hit by the player, optionally destroy the player.
	player->youHit(this);
}

void SnakeBodySegment::youHit(Missile *missile) {
	static NovaColor darkGrey = NovaColor(64, 64, 64);

	// This segment was hit by the player's missile.
	if (!this->isDisabled()) {
		if (this->spriteColor.fade(DAMAGE_FACTOR, lowColor)) {
			this->setSpriteColor(darkGrey);
			this->setDisabled(true);
		}
	}

	// Missile has been destroyed.
	playState->getMissileController()->deactivate(missile);
}
