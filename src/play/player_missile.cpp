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

PlayerMissile::PlayerMissile(PlayState *playState) : Missile(playState) {
	missileType = PLAYER_MISSILE;
	this->setSpriteDefinition("missile");
	this->setSpriteColor(NovaColor(253, 243, 2));
	this->setExplosionColor(this->getSpriteColor());
	homingEnabled = false;
	target = NULL;
}

void PlayerMissile::setActive(bool active) {
	// Base processing.
	Missile::setActive(active);

	if (active) {
		// Reset.
		homingEnabled = false;
		target = NULL;
	}
}

void PlayerMissile::update(float elapsedTime) {
	static NovaVertex targetPosition;
	static NovaVertex missilePosition;

    if (homingEnabled) {
        if (!target || (!target->isActive())) {
            // Need to find a new target.
            target = playState->getAlienController()->findBestTarget(this);
        }

        if (target) {
        	targetPosition = target->getPositionWCS();
    		missilePosition = this->getPositionWCS();

			// Update the velocity and direction of the missile so that it chases the moving target.
			double targetDirection = calculateDirectionFromVelocityComponents((targetPosition.x - missilePosition.x),
				(targetPosition.y - missilePosition.y));

			this->setVelocityFromDirection(targetDirection, this->getTotalVelocity());

			// Rotate the sprite to match the direction of travel.
			this->setFacingTowardsDirection(targetDirection);
	   }
    }

    // Do the base processing.
    Missile::update(elapsedTime);
}
