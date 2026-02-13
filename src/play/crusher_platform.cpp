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
#define LEFT_RETREAT_POINT_X -130
#define RIGHT_RETREAT_POINT_X 130

CrusherPlatform::CrusherPlatform(PlayState* playState, Sprite* parent) : AlienComponent(playState, parent) {
	this->setSpriteDefinition("crusher_platform");
	this->setDefaultColor(NovaColor(207, 4, 190));
	retreating = false;

	// Create the invisible bounding spheres.
	boundingSpheres.push_back(new BoundingSphere(playState, this, FIRST_SPHERE_VERTEX_ANCHOR, BOUNDING_SPHERE_SIZE));
	boundingSpheres.push_back(new BoundingSphere(playState, this, SECOND_SPHERE_VERTEX_ANCHOR, BOUNDING_SPHERE_SIZE));
	boundingSpheres.push_back(new BoundingSphere(playState, this, THIRD_SPHERE_VERTEX_ANCHOR, BOUNDING_SPHERE_SIZE));
	boundingSpheres.push_back(new BoundingSphere(playState, this, FOURTH_SPHERE_VERTEX_ANCHOR, BOUNDING_SPHERE_SIZE));

	// Create the guns.
	gunPlatforms.push_back(new GunPlatform(playState, this, FIRST_GUN_VERTEX_ANCHOR));
	gunPlatforms.push_back(new GunPlatform(playState, this, SECOND_GUN_VERTEX_ANCHOR));
	gunPlatforms.push_back(new GunPlatform(playState, this, THIRD_GUN_VERTEX_ANCHOR));
	gunPlatforms.push_back(new GunPlatform(playState, this, FOURTH_GUN_VERTEX_ANCHOR));
}

CrusherPlatform::~CrusherPlatform() {
	for (BoundingSphere* component : boundingSpheres) {
		delete component;
	}

	boundingSpheres.clear();

	for (GunPlatform* component : gunPlatforms) {
		delete component;
	}

	gunPlatforms.clear();
}

void CrusherPlatform::setActive(bool active) {
	// Base processing.
	AlienComponent::setActive(active);

	if (active) {
		// Reset.
		retreating = false;
	}

	for (AlienComponent* component : gunPlatforms) {
		component->setActive(active);
	}
}

void CrusherPlatform::update(float elapsedTime) {
	for (AlienComponent* component : boundingSpheres) {
		component->update(elapsedTime);
	}

	for (AlienComponent* component : gunPlatforms) {
		component->update(elapsedTime);
	}

	// If any guns are visible then mark the container as visible (so the draw method is called).
	this->setVisible(false);
	for (AlienComponent* component : gunPlatforms) {
		if (component->isVisible()) {
			this->setVisible(true);
			break;
		}
	}
}

bool CrusherPlatform::checkCollision(Player* player) {
	bool collision = false;

	// See if this platform has just hit the player.
	for (AlienComponent* component : boundingSpheres) {
		collision = component->checkCollision(player);
		if (collision) {
			break;
		}
	}

	if (!collision) {
		for (AlienComponent* component : gunPlatforms) {
			collision = component->checkCollision(player);
			if (collision) {
				break;
			}
		}
	}

	return collision;
}

bool CrusherPlatform::checkCollision(Missile* missile) {
	bool collision = false;

	// See if this platform has just hit this missile.
	for (AlienComponent* component : boundingSpheres) {
		collision = component->checkCollision(missile);
		if (collision) {
			break;
		}
	}

	if (!collision) {
		for (AlienComponent* component : gunPlatforms) {
			collision = component->checkCollision(missile);
			if (collision) {
				break;
			}
		}
	}

	// See if this platform has just been disabled.
	if (collision && (this->isActive())) {
		bool disabled = true;

		for (AlienComponent* component : gunPlatforms) {
			if (component->isActive()) {
				disabled = false;
				break;
			}
		}

		if (disabled) {
			this->setActive(false);
		}
	}

	return collision;
}

void CrusherPlatform::smartBombNotification() {
	for (AlienComponent* component : gunPlatforms) {
		if (component->isActive()) {
			component->setActive(false);
			break;
		}
	}

	// See if this platform has just been disabled.
	bool disabled = true;

	for (AlienComponent* component : gunPlatforms) {
		if (component->isActive()) {
			disabled = false;
			break;
		}
	}

	if (disabled) {
		this->setActive(false);
	}
}

void CrusherPlatform::draw() {
	// Base processing.
	AlienComponent::draw();

	for (AlienComponent* component : gunPlatforms) {
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
	if (retreating) {
		this->setVelocityFromComponents(-1.2, 0);
		this->applyVelocity(elapsedTime);

		// Move off the screen.
		if (this->getPositionWCS().x < LEFT_RETREAT_POINT_X) {
			// Finished retreating.
			retreating = false;
		}
	} else {
		Player* player = playState->getPlayer();
		if ((!player->isActive())) {
			if (this->getPositionWCS().x > LEFT_SPAWN_POINT_X) {
				this->setVelocityFromComponents(-1.2, 0);
				this->applyVelocity(elapsedTime);

				// Only move back to the starting position.
				if (this->getPositionWCS().x < LEFT_SPAWN_POINT_X) {
					this->moveTo(LEFT_SPAWN_POINT_X, 0, 0);
				}
			}
		} else {
			this->setVelocityFromComponents(0.4, 0);
			this->applyVelocity(elapsedTime);
		}
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
	if (retreating) {
		this->setVelocityFromComponents(1.2, 0);
		this->applyVelocity(elapsedTime);

		// Move off the screen.
		if (this->getPositionWCS().x > RIGHT_RETREAT_POINT_X) {
			// Finished retreating.
			retreating = false;
		}
	} else {
		Player* player = playState->getPlayer();
		if ((!player->isActive())) {
			if (this->getPositionWCS().x < RIGHT_SPAWN_POINT_X) {
				this->setVelocityFromComponents(1.2, 0);
				this->applyVelocity(elapsedTime);

				// Only move back to the starting position.
				if (this->getPositionWCS().x > RIGHT_SPAWN_POINT_X) {
					this->moveTo(RIGHT_SPAWN_POINT_X, 0, 0);
				}
			}
		} else {
			this->setVelocityFromComponents(-0.4, 0);
			this->applyVelocity(elapsedTime);
		}
	}

	// Base processing.
	CrusherPlatform::update(elapsedTime);
}




