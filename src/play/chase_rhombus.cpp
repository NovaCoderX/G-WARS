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

#define WARP_ANIMATION_SPEED 0.6f

static float lastAnimatedTime = 0;
static bool expandingWarp = true;
static float currentWarpSize = 0;
static float minWarpSize = 0;
static float maxWarpSize = 0;

ChaseRhombus::ChaseRhombus(PlayState* playState) : ChaseAlien(playState, CHASE_RHOMBUS_ALIEN) {
	this->setSpriteDefinition("chase_rhombus");
	this->setDefaultColor(NovaColor(0, 150, 255));

	if (!currentWarpSize) {
		currentWarpSize = this->definition->staticVertices[2].y;
		minWarpSize = currentWarpSize - (currentWarpSize / 2);
		maxWarpSize = currentWarpSize + (currentWarpSize / 2);
	}
}

void ChaseRhombus::update(float elapsedTime) {
	// Base processing.
	ChaseAlien::update(elapsedTime);

	if (this->isVisible()) {
		if (elapsedTime != lastAnimatedTime) {
			if (expandingWarp) {
				currentWarpSize += (WARP_ANIMATION_SPEED * elapsedTime);
				if (currentWarpSize >= maxWarpSize) {
					currentWarpSize = maxWarpSize;
					expandingWarp = false;
				}

				this->definition->staticVertices[0].y = -currentWarpSize;
				this->definition->staticVertices[2].y = currentWarpSize;
			} else {
				currentWarpSize -= (WARP_ANIMATION_SPEED * elapsedTime);
				if (currentWarpSize <= minWarpSize) {
					currentWarpSize = minWarpSize;
					expandingWarp = true;
				}

				this->definition->staticVertices[0].y = -currentWarpSize;
				this->definition->staticVertices[2].y = currentWarpSize;
			}

			// We also need to update the shared definition's bounding sphere to reflect the new size.
			this->definition->calculateBoundingSphere();

			lastAnimatedTime = elapsedTime;
		}
	}
}
