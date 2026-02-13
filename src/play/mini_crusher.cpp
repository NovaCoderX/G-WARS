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

#define MIN_INITIAL_VELOCITY 8
#define MAX_INITIAL_VELOCITY 12

MiniCrusher::MiniCrusher(PlayState* playState) : Alien(playState, MINI_CRUSHER_ALIEN) {
	// The alien container doesn't explode.
	this->setExplosionSize(NO_EXPLOSION);

	highColor = NovaColor(3, 202, 252);
	lowColor = highColor;
	lowColor.rebase(30);
	this->setDefaultColor(lowColor);
	increasingColor = true;

	top = new AlienComponent(playState, this);
	top->setSpriteDefinition("mini_crusher");
	top->setExplosionSize(MEDIUM_EXPLOSION);
	top->setExplosionSound(SPECIAL_ALIEN_EXPLODE);
	top->setExplosionColor(highColor);
	top->setFacingTowardsDirection(DOWN);

	bottom = new AlienComponent(playState, this);
	bottom->setSpriteDefinition("mini_crusher");
	bottom->setExplosionSize(MEDIUM_EXPLOSION);
	bottom->setExplosionSound(SPECIAL_ALIEN_EXPLODE);
	bottom->setExplosionColor(highColor);
	this->setExplosionSound(SPECIAL_ALIEN_EXPLODE);
}

MiniCrusher::~MiniCrusher() {
	if (top) {
		delete top;
		top = NULL;
	}

	if (bottom) {
		delete bottom;
		bottom = NULL;
	}
}

void MiniCrusher::setActive(bool active) {
	// Base processing.
	Alien::setActive(active);

	if (active) {
		// Make some sound.
		g_worldManager->startSound(ENEMY_SPAWN_MINI_CRUSHER);

		// Set initial position, direction and velocity of the new alien.
		this->moveTo(0, 0, 0);
		this->setVelocityFromComponents(0, 0);

		// Setup the top and bottom platforms.
		float x = float_rand(60, 90);
		float initialVelocity = float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY);

		// Find out which spawn zone we should be using (based on the players current location).
		if (playState->getPlayAreaController()->isWithinZone(ZoneIndex::LEFT_SPAWN_ZONE, playState->getPlayer())) {
			// Spawn the alien in the right zone.
			top->moveTo(x, TOP_SPAWN_POINT_Y, 0);
			top->setVelocityFromComponents(0, -initialVelocity);

			bottom->moveTo(x, BOTTOM_SPAWN_POINT_Y, 0);
			bottom->setVelocityFromComponents(0, initialVelocity);
		} else {
			// Spawn the alien in the left zone.
			top->moveTo(-x, TOP_SPAWN_POINT_Y, 0);
			top->setVelocityFromComponents(0, -initialVelocity);

			bottom->moveTo(-x, BOTTOM_SPAWN_POINT_Y, 0);
			bottom->setVelocityFromComponents(0, initialVelocity);
		}

		// Reset.
		increasingColor = true;
	}

	top->setActive(active);
	bottom->setActive(active);
}

void MiniCrusher::update(float elapsedTime) {
	// Apply the velocity.
	top->applyVelocity(elapsedTime);

	if (top->getVerticalVelocity() > 0) {
		if (!playState->getPlayAreaController()->isWithinPlayArea(TOP_BORDER, top)) {
			// We just hit the top border, make vertical velocity negative.
			top->reverseVerticalVelocity();
		}
	} else {
		// We just hit the middle of the screen, make vertical velocity positive.
		if ((top->getPositionWCS().y - top->getBoundingSphere()) < 0) {
			top->reverseVerticalVelocity();
		}
	}

	// Apply the velocity.
	bottom->applyVelocity(elapsedTime);

	// See if the bottom just moved outside the play area.
	if (bottom->getVerticalVelocity() > 0) {
		// We just hit the middle of the screen, make vertical velocity positive.
		if ((bottom->getPositionWCS().y + bottom->getBoundingSphere()) > 0) {
			bottom->reverseVerticalVelocity();
		}
	} else {
		// We just hit the bottom border, make vertical velocity negative.
		if (!playState->getPlayAreaController()->isWithinPlayArea(BOTTOM_BORDER, bottom)) {
			bottom->reverseVerticalVelocity();
		}
	}

	// Mark the platform objects visible/invisible for this frame.
	top->calculateVisibility();
	bottom->calculateVisibility();

	// If either platform is visible, then mark the container as visible (so the draw method is called).
	this->setVisible(false);
	if (top->isVisible() || bottom->isVisible()) {
		this->setVisible(true);
	}


	// Animate.
	if (this->isVisible()) {
		// Do some simple color cycling.
		if (increasingColor) {
			if (currentColor.brighten((elapsedTime / 10), highColor)) {
				increasingColor = false;
			}
		} else {
			if (currentColor.fade((elapsedTime / 10), lowColor)) {
				increasingColor = true;
			}
		}

		top->setCurrentColor(this->getCurrentColor());
		bottom->setCurrentColor(this->getCurrentColor());
	}
}

bool MiniCrusher::checkCollision(Player* player) {
	bool collision = false;

	// See if the top has just hit the player.
	collision = top->checkCollision(player);

	// Also check the bottom.
	if (!collision) {
		collision = bottom->checkCollision(player);
	}

	if (collision) {
		// This alien was hit by the player and destroyed.
		playState->getAlienController()->deactivate(this);

		// Optionally destroy the player.
		player->youHit(this);
	}

	return collision;
}

bool MiniCrusher::checkCollision(Missile* missile) {
	bool collision = false;

	// See if the top has just hit this missile.
	collision = top->checkCollision(missile);

	// Also check the bottom.
	if (!collision) {
		collision = bottom->checkCollision(missile);
	}

	if (collision) {
		// This alien was hit by the player's missile.
		playState->getAlienController()->deactivate(this);
		// Note, cannot spawn nuggets because of container WCS position?
		// would need to change Alien class to Sprite to allow each AlienComponent to spawn a nugget.
		// If we do this then even the snake could spawn nuggets for each segment?
		playState->getPlayer()->increaseScore(this);

		// Missile has also been destroyed.
		playState->getMissileController()->deactivate(missile);
	}

	return collision;
}

void MiniCrusher::draw() {
	if (top->isVisible()) {
		top->draw();
	}

	if (bottom->isVisible()) {
		bottom->draw();
	}
}

