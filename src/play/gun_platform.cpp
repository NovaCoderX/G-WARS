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

#define MAX_DAMAGE 5
#define INITIAL_FIRE_DELAY 30
#define FIRE_INTERVAL 30
#define MAXIMUM_TARGET_ANGLE 20
#define MUZZLE_VERTEX_ANCHOR 9

GunPlatform::GunPlatform(PlayState* playState, Sprite* parent, int anchorIndex) : AlienComponent(playState, parent) {
	this->anchorIndex = anchorIndex;
	this->setSpriteDefinition("gun_platform");
	defaultPlatformColor = NovaColor(105, 19, 176);
	this->setSpriteColor(defaultPlatformColor);
	this->setExplosionSize(MEDIUM_EXPLOSION);
	this->setExplosionColor(this->getSpriteColor());
	damage = 0;
	totalElapsedTime = lastFireTime = 0;

	// Create turret.
	gunTurret = new Sprite(playState);
	gunTurret->setSpriteDefinition("platform_gun");
	defaultGunColor = NovaColor(247, 5, 49);
	gunTurret->setSpriteColor(defaultGunColor);
	disabledColor = NovaColor(64, 64, 64);
}

GunPlatform::~GunPlatform() {
	if (gunTurret) {
		delete gunTurret;
		gunTurret = NULL;
	}
}

void GunPlatform::setDisabled(bool disabled) {
	// Base processing.
	AlienComponent::setDisabled(disabled);

	if (disabled) {
		this->setSpriteColor(disabledColor);
		gunTurret->setSpriteColor(disabledColor);
	} else {
		this->setSpriteColor(defaultPlatformColor);
		gunTurret->setSpriteColor(defaultGunColor);
	}
}

void GunPlatform::setActive(bool active) {
	// Base processing.
	AlienComponent::setActive(active);

	if (active) {
		// Reset.
		damage = 0;
		totalElapsedTime = lastFireTime = 0;
	}
}

void GunPlatform::update(float elapsedTime) {
	totalElapsedTime += elapsedTime;

	this->moveTo(parent->getAnchorPointWCS(anchorIndex));

	// Mark this object visible/invisible for this frame.
	this->calculateVisibility();

	if (this->visible) {
		gunTurret->moveTo(this->getPositionWCS());
		if (!this->isDisabled()) {
			Player *player = playState->getPlayer();
			if (player->isActive()) {
				NovaVertex playerPosition = player->getPositionWCS();
				NovaVertex alienPosition = gunTurret->getPositionWCS();
				double playerDirection = calculateDirectionFromVelocityComponents((playerPosition.x - alienPosition.x),
						(playerPosition.y - alienPosition.y));

				// Rotate the turret to point towards the player.
				gunTurret->setFacingTowardsDirection(playerDirection);

				// See if we are now ready to fire.
				if (totalElapsedTime > INITIAL_FIRE_DELAY) {
					if ((totalElapsedTime - lastFireTime) >= FIRE_INTERVAL) {
						// Need to check to see if we are traveling roughly in the correct direction to fire.
						if (fabs(gunTurret->getFacingTowardsDirection() - playerDirection) < MAXIMUM_TARGET_ANGLE) {
							playState->getMissileController()->launchMissile(gunTurret->getAnchorPointWCS(MUZZLE_VERTEX_ANCHOR),
									playerDirection, 2.0);

							// Store the last time that we fired.
							lastFireTime = totalElapsedTime;
						}
					}
				}
			}
		}
	}
}

bool GunPlatform::checkCollision(Missile* missile) {
	bool collision = false;

	NovaVertex between = (missile->getPositionWCS() - this->getPositionWCS());
	if (between.magnitude() < (missile->getBoundingSphere() + this->getBoundingSphere())) {
		// This segment was hit by the player's missile.
		collision = true;

		if (!this->isDisabled()) {
			damage++;
			if (damage >= MAX_DAMAGE) {
				this->setDisabled(true);
			}
		}
	}

	return collision;
}

void GunPlatform::draw() {
	// Do base processing to draw the base.
	AlienComponent::draw();

	// If the platform is visible, then so is the gun.
	gunTurret->draw();
}
