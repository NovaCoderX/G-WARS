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

#define BOUNDING_SPHERE_SIZE 6
#define FIRST_SPHERE_VERTEX_ANCHOR 15
#define SECOND_SPHERE_VERTEX_ANCHOR 16
#define THIRD_SPHERE_VERTEX_ANCHOR 17
#define FOURTH_SPHERE_VERTEX_ANCHOR 18
#define FIRST_GUN_VERTEX_ANCHOR 19
#define SECOND_GUN_VERTEX_ANCHOR 20
#define THIRD_GUN_VERTEX_ANCHOR 21
#define FOURTH_GUN_VERTEX_ANCHOR 22

CrusherPlatform::CrusherPlatform(PlayState* playState, Sprite* parent) : AlienComponent(playState, parent) {
	this->setSpriteDefinition("crusher_platform");
	defaultColor = NovaColor(207, 4, 190);
	disabledColor = NovaColor(64, 64, 64);
	this->setSpriteColor(defaultColor);
	this->setExplosionSize(LARGE_EXPLOSION);
	this->setExplosionColor(this->getSpriteColor());

	// Create the invisible bounding spheres.
	components.push_back(new BoundingSphere(playState, this, FIRST_SPHERE_VERTEX_ANCHOR, BOUNDING_SPHERE_SIZE));
	components.push_back(new BoundingSphere(playState, this, SECOND_SPHERE_VERTEX_ANCHOR, BOUNDING_SPHERE_SIZE));
	components.push_back(new BoundingSphere(playState, this, THIRD_SPHERE_VERTEX_ANCHOR, BOUNDING_SPHERE_SIZE));
	components.push_back(new BoundingSphere(playState, this, FOURTH_SPHERE_VERTEX_ANCHOR, BOUNDING_SPHERE_SIZE));

	// Create the guns.
	components.push_back(new GunPlatform(playState, this, FIRST_GUN_VERTEX_ANCHOR));
	components.push_back(new GunPlatform(playState, this, SECOND_GUN_VERTEX_ANCHOR));
	components.push_back(new GunPlatform(playState, this, THIRD_GUN_VERTEX_ANCHOR));
	components.push_back(new GunPlatform(playState, this, FOURTH_GUN_VERTEX_ANCHOR));
}

CrusherPlatform::~CrusherPlatform() {
	for (AlienComponent* component : components) {
		delete component;
	}

	components.clear();
}

void CrusherPlatform::setDisabled(bool disabled) {
	// Base processing.
	AlienComponent::setDisabled(disabled);

	if (disabled) {
		this->setSpriteColor(disabledColor);
	} else {
		this->setSpriteColor(defaultColor);
	}
}

void CrusherPlatform::setActive(bool active) {
	// Base processing.
	AlienComponent::setActive(active);

	for (AlienComponent* component : components) {
		component->setActive(active);
	}
}

void CrusherPlatform::update(float elapsedTime) {
	for (AlienComponent* component : components) {
		component->update(elapsedTime);
	}

	// If any guns are visible then mark the container as visible (so the draw method is called).
	this->setVisible(false);
	for (AlienComponent* component : components) {
		if (component->isVisible()) {
			this->setVisible(true);
			break;
		}
	}
}

bool CrusherPlatform::checkCollision(Player* player) {
	bool collision = false;

	// See if this platform has just hit the player.
	for (AlienComponent* component : components) {
		collision = component->checkCollision(player);
		if (collision) {
			break;
		}
	}

	return collision;
}

bool CrusherPlatform::checkCollision(Missile* missile) {
	bool collision = false;

	// See if this platform has just hit this missile.
	for (AlienComponent* component : components) {
		collision = component->checkCollision(missile);
		if (collision) {
			break;
		}
	}

	if (collision && (!this->isDisabled())) {
		bool disabled = true;

		// See if this platform has just been disabled.
		for (AlienComponent* component : components) {
			if (!component->isDisabled()) {
				disabled = false;
				break;
			}
		}

		if (disabled) {
			this->setDisabled(true);
		}
	}

	return collision;
}

void CrusherPlatform::draw() {
	// Base processing.
	AlienComponent::draw();

	for (AlienComponent* component : components) {
		if (component->isVisible()) {
			component->draw();
		}
	}
}

/*****************************************************************************
 LeftCrusherPlatform
*****************************************************************************/
LeftCrusherPlatform::LeftCrusherPlatform(PlayState* playState, Sprite* parent) : CrusherPlatform(playState, parent) {
	// Empty.
}

void LeftCrusherPlatform::setActive(bool active) {
	// Base processing.
	CrusherPlatform::setActive(active);

	if (active) {
		// Move to the starting position.
		this->moveTo(LEFT_SPAWN_POINT_X, 0, 0);

		// Rotate the sprite to match the direction of travel.
		this->setFacingTowardsDirection(RIGHT);
	}
}

void LeftCrusherPlatform::update(float elapsedTime) {
	Player* player = playState->getPlayer();

	if ((!player->isActive())) {
		if (this->getPositionWCS().x > LEFT_SPAWN_POINT_X) {
			this->setVelocityFromComponents(-1.2, 0);
			this->applyVelocity(elapsedTime);

			// Only move to the starting position.
			if (this->getPositionWCS().x < LEFT_SPAWN_POINT_X) {
				this->moveTo(LEFT_SPAWN_POINT_X, 0, 0);
			}
		}
	} else {
		this->setVelocityFromComponents(0.4, 0);
		this->applyVelocity(elapsedTime);
	}

	// Base processing.
	CrusherPlatform::update(elapsedTime);
}

/*****************************************************************************
 RightCrusherPlatform
*****************************************************************************/
RightCrusherPlatform::RightCrusherPlatform(PlayState* playState, Sprite* parent) : CrusherPlatform(playState, parent) {
	// Empty.
}

void RightCrusherPlatform::setActive(bool active) {
	// Base processing.
	CrusherPlatform::setActive(active);

	if (active) {
		// Move to the starting position.
		this->moveTo(RIGHT_SPAWN_POINT_X, 0, 0);

		// Rotate the sprite to match the direction of travel.
		this->setFacingTowardsDirection(LEFT);
	}
}

void RightCrusherPlatform::update(float elapsedTime) {
	Player* player = playState->getPlayer();

	if ((!player->isActive())) {
		if (this->getPositionWCS().x < RIGHT_SPAWN_POINT_X) {
			this->setVelocityFromComponents(1.2, 0);
			this->applyVelocity(elapsedTime);

			// Only move to the starting position.
			if (this->getPositionWCS().x > RIGHT_SPAWN_POINT_X) {
				this->moveTo(RIGHT_SPAWN_POINT_X, 0, 0);
			}
		}
	} else {
		this->setVelocityFromComponents(-0.4, 0);
		this->applyVelocity(elapsedTime);
	}

	// Base processing.
	CrusherPlatform::update(elapsedTime);
}




