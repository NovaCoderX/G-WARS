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

#define SPIN_ANIMATION_SPEED 16.0f

static AstMatrix3x3 rotationMatrix;
static float lastAnimatedTime = 0;

BounceStar::BounceStar(PlayState* playState) : BounceAlien(playState) {
	this->alienType = BOUNCE_STAR;
	this->setSpriteDefinition("bounce_star");
	this->setSpriteColor(NovaColor(197, 122, 250));
	this->setExplosionColor(this->getSpriteColor());
}

void BounceStar::update(float elapsedTime) {
	// Base processing.
	BounceAlien::update(elapsedTime);

	if (this->isVisible()) {
		if (elapsedTime != lastAnimatedTime) {
			if (this->horizontalVelocity > 0) {
				rotationMatrix.MakeZRotation(-(SPIN_ANIMATION_SPEED * elapsedTime));

				for (int i = 0; i < this->definition->getNumVertices(); i++) {
					this->definition->staticVertices[i] = (this->definition->staticVertices[i] * rotationMatrix);
				}
			} else {
				rotationMatrix.MakeZRotation(SPIN_ANIMATION_SPEED * elapsedTime);

				for (int i = 0; i < this->definition->getNumVertices(); i++) {
					this->definition->staticVertices[i] = (this->definition->staticVertices[i] * rotationMatrix);
				}
			}

			lastAnimatedTime = elapsedTime;
		}
	}
}
