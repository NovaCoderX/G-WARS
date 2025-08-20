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

#define FIRST_TENTACLE_VERTEX_ANCHOR 19
#define SECOND_TENTACLE_VERTEX_ANCHOR 20
#define THIRD_TENTACLE_VERTEX_ANCHOR 21
#define FOURTH_TENTACLE_VERTEX_ANCHOR 22

Jelly::Jelly(PlayState* playState) : Alien(playState) {
	this->setAlienType(JELLY);
	this->setSpriteDefinition("jelly");
	highColor = NovaColor(48, 160, 255);
	lowColor = highColor;
	lowColor.rebase(33);
	this->setSpriteColor(highColor);
	increasingColor = false;
	this->setExplosionSize(LARGE_EXPLOSION);
	this->setExplosionColor(this->getSpriteColor());

	// Does not spawn nuggets.
	this->setNuggetSpawnType(DO_NOT_SPAWN);

	// Create the tentacles.
	tentacles.push_back(new Tentacle(this, FIRST_TENTACLE_ANCHOR, NovaColor(252, 7, 35)));
	tentacles.push_back(new Tentacle(this, SECOND_TENTACLE_ANCHOR, NovaColor(251, 42, 245)));
	tentacles.push_back(new Tentacle(this, THIRD_TENTACLE_ANCHOR, NovaColor(237, 49, 129)));
	tentacles.push_back(new Tentacle(this, FOURTH_TENTACLE_ANCHOR, NovaColor(179, 83, 252)));
}

Jelly::~Jelly() {
	for (Tentacle* tentacle : tentacles) {
		delete tentacle;
	}

	tentacles.clear();
}

void Jelly::setActive(bool active) {
	// Base processing.
	Alien::setActive(active);

	if (active) {
		// Make some sound.
		g_worldManager->startSound(ENEMY_SPAWN_BOSS);

		// Reset.
		this->setSpriteColor(highColor);
		increasingColor = false;
	}

	for (Tentacle* tentacle : tentacles) {
		tentacle->setActive(active);
	}
}

void Jelly::update(float elapsedTime) {
	// Apply the velocity.
	this->applyVelocity(elapsedTime);

	// See if we have just moved outside the play area.
	if (this->getHorizontalVelocity() > 0) {
		if (!playState->getPlayAreaController()->isWithinPlayArea(RIGHT_BORDER, this)) {
			// We just hit the right border.
			this->reverseHorizontalVelocity();
		}
	} else {
		if (!playState->getPlayAreaController()->isWithinPlayArea(LEFT_BORDER, this)) {
			// We just hit the left border.
			this->reverseHorizontalVelocity();
		}
	}

	if (this->getVerticalVelocity() > 0) {
		if (this->getPositionWCS().y > 80) {
			// We just hit the top border, make vertical velocity negative.
			this->reverseVerticalVelocity();
		}
	} else {
		if (this->getPositionWCS().y < 0) {
			// We just hit the middle of the screen, make vertical velocity positive.
			this->reverseVerticalVelocity();
		}
	}

	for (Tentacle* tentacle : tentacles) {
		tentacle->update(elapsedTime);
	}

	// See if the bell is visible.
	this->calculateVisibility();
	if (!this->isVisible()) {
		// The bell isn't visible, see if any tentacles are visible.
		for (Tentacle* tentacle : tentacles) {
			if (tentacle->isVisible()) {
				// Mark the container as visible (so the draw method is called).
				this->setVisible(true);
				break;
			}
		}
	}

	if (this->isVisible()) {
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
}

bool Jelly::checkCollision(Player* player) {
	bool collision = false;

	// Check the tentacles first.
	for (Tentacle* tentacle : tentacles) {
		if (tentacle->isVisible()) {
			collision = tentacle->checkCollision(player);
			if (collision) {
				break;
			}
		}
	}

	if (!collision) {
		// If no collision, check the bell.
		NovaVertex between = (player->getPositionWCS() - this->getPositionWCS());
		if (between.magnitude() < (player->getBoundingSphere() + this->getBoundingSphere())) {
			collision = true;
		}
	}

	if (collision) {
		// This alien was hit by the player, optionally destroy the player.
		player->youHit(this);
	}

	return collision;
}

bool Jelly::checkCollision(Missile* missile) {
	bool collision = false;

	// Check the tentacles first.
	for (Tentacle* tentacle : tentacles) {
		if (tentacle->isVisible()) {
			collision = tentacle->checkCollision(missile);
			if (collision) {
				break;
			}
		}
	}

	if (!collision) {
		// If no collision, check the bell.
		NovaVertex between = (missile->getPositionWCS() - this->getPositionWCS());
		if (between.magnitude() < (missile->getBoundingSphere() + this->getBoundingSphere())) {
			collision = true;
		}
	}

	if (collision) {
		bool disabled = true;
		for (Tentacle* tentacle : tentacles) {
			if (!tentacle->isDisabled()) {
				disabled = false;
				break;
			}
		}

		if (disabled) {
			// All the tentacles are dead, therefore the jelly is dead.
			playState->getAlienController()->deactivate(this);
			playState->getPlayer()->increaseScore(this);
		}

		// Missile has been destroyed.
		playState->getMissileController()->deactivate(missile);
	}

	return collision;
}

void Jelly::draw() {
	// Base processing.
	Alien::draw();

	for (Tentacle* tentacle : tentacles) {
		if (tentacle->isVisible()) {
			tentacle->draw();
		}
	}
}

NovaVertex Jelly::getAnchorPointWCS(AnchorPoint anchor) const {
	uint index;

	switch (anchor) {
	case FIRST_TENTACLE_ANCHOR:
		index = FIRST_TENTACLE_VERTEX_ANCHOR;
		break;

	case SECOND_TENTACLE_ANCHOR:
		index = SECOND_TENTACLE_VERTEX_ANCHOR;
		break;

	case THIRD_TENTACLE_ANCHOR:
		index = THIRD_TENTACLE_VERTEX_ANCHOR;
		break;

	default: // FOURTH_TENTACLE_ANCHOR
		index = FOURTH_TENTACLE_VERTEX_ANCHOR;
	}

	// Transform.
	return (this->definition->staticVertices[index] * objectToWorldMatrix);
}

