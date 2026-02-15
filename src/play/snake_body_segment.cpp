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

#define DAMAGE_FACTOR 0.25
#define FOLLOW_DISTANCE 3.0

SnakeBodySegment::SnakeBodySegment(PlayState* playState, Sprite* parent, const NovaColor &color) : SnakeSegment(playState, parent, color) {
	// Set up how this object will explode.
	this->setExplosionSize(LARGE_EXPLOSION);
	this->setExplosionSound(SILENCE);
	this->setExplosionColor(this->getDefaultColor());
}

void SnakeBodySegment::update(float elapsedTime) {
	static NovaVertex alienPosition;
	static NovaVertex parentPosition;

	// Always move towards the parent object.
	alienPosition = this->getPositionWCS();
	parentPosition = parent->getPositionWCS();

	float currentDistance = (parentPosition - alienPosition).magnitude();
	if (currentDistance > FOLLOW_DISTANCE) {
		double parentDirection = calculateDirectionFromVelocityComponents((parentPosition.x - alienPosition.x),
				(parentPosition.y - alienPosition.y));
		double horzVelocity = parentPosition.x - alienPosition.x;
		double vertVelocity = parentPosition.y - alienPosition.y;
		double velocity = sqrt(horzVelocity * horzVelocity + vertVelocity * vertVelocity);
		this->setVelocityFromDirection(parentDirection, velocity);
		this->setFacingTowardsDirection(parentDirection);
	}

	// Apply the velocity.
	this->applyVelocity(elapsedTime);

	// Mark this object visible/invisible for this frame.
	this->calculateVisibility();
}

bool SnakeBodySegment::checkCollision(Missile* missile) {
	bool collision = false;

	NovaVertex between = (missile->getPositionWCS() - this->getPositionWCS());
	if (between.magnitude() < (missile->getBoundingSphere() + this->getBoundingSphere())) {
		// This segment was hit by the player's missile.
		collision = true;

		if (this->isActive()) {
			if (this->currentColor.fade(DAMAGE_FACTOR, this->getDisabledtColor())) {
				this->setActive(false);
			}
		}
	}

	return collision;
}


