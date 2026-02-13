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

#include <limits>

#define MAX_DAMAGE 5
#define MAXIMUM_TARGET_ANGLE 20
#define MAXIMUM_TARGET_DISTANCE 60

// Function to calculate the first border hit by the laser.
static std::pair<int, int> first_border_hit(double x0, double y0, double radians, double width, double height) {
	// Direction of the laser (cosine and sine of the angle)
	double dx = cos(radians);
	double dy = sin(radians);

	// Initialize t values to some large value (we'll minimize these)
	double t_right = (width - x0) / dx;
	double t_left = (-width - x0) / dx;
	double t_top = (height - y0) / dy;
	double t_bottom = (-height - y0) / dy;

	// Ensure that we're calculating the correct t values (only positive t values)
	if (dx == 0) {
		t_right = t_left = std::numeric_limits<double>::infinity();
	}
	if (dy == 0) {
		t_top = t_bottom = std::numeric_limits<double>::infinity();
	}

	// Check for which border the laser hits first (smallest positive t)
	double t_min = std::numeric_limits<double>::infinity();
	std::pair<int, int> hit_coords = { 0, 0 }; // @suppress("Invalid arguments")

	if (t_right > 0 && t_right < t_min) {
		t_min = t_right;
		hit_coords = { static_cast<int>(x0 + t_min * dx), static_cast<int>(y0 + t_min * dy) };
	}

	if (t_left > 0 && t_left < t_min) {
		t_min = t_left;
		hit_coords = { static_cast<int>(x0 + t_min * dx), static_cast<int>(y0 + t_min * dy) };
	}

	if (t_top > 0 && t_top < t_min) {
		t_min = t_top;
		hit_coords = { static_cast<int>(x0 + t_min * dx), static_cast<int>(y0 + t_min * dy) };
	}

	if (t_bottom > 0 && t_bottom < t_min) {
		t_min = t_bottom;
		hit_coords = { static_cast<int>(x0 + t_min * dx), static_cast<int>(y0 + t_min * dy) };
	}

	return hit_coords;
}

static bool checkLaserHit(double x0, double y0, double radians, double targetX, double targetY, double radius) {
	// Laser direction components
	double vx = cos(radians);
	double vy = sin(radians);

	// Step through time increments to check if laser enters the bounding sphere
	double step = 10;  // Time step granularity (you can tweak this)
	for (double t = 0; t < 100; t += step) {  // 100 is arbitrary; you can replace with a max range
		double x = x0 + vx * t;
		double y = y0 + vy * t;

		double distanceSquared = (x - targetX) * (x - targetX) + (y - targetY) * (y - targetY);
		if (distanceSquared <= radius * radius) {
			return true;  // Hit detected within radius
		}
	}

	return false;  // No hit detected
}

TentacleNematocyst::TentacleNematocyst(PlayState* playState, Sprite* parent, float radius, const NovaColor& color) :
		TentacleSegment(playState, parent, radius, color) {
	highColor = color;
	lowColor = highColor;
	lowColor.rebase(10);
	this->setDefaultColor(lowColor);
	increasingColor = true;

	// Set up how this object will explode.
	this->setExplosionSize(LARGE_EXPLOSION);
	this->setExplosionColor(highColor);

	damage = 0;

	laser = new Laser(playState);
	laser->setColor(color);
}

TentacleNematocyst::~TentacleNematocyst() {
	if (laser) {
		delete laser;
		laser = NULL;
	}
}

void TentacleNematocyst::setActive(bool active) {
	// Base processing.
	TentacleSegment::setActive(active);

	if (active) {
		// Reset.
		increasingColor = true;
		damage = 0;
		laser->setActive(false);
	}
}

void TentacleNematocyst::update(float elapsedTime) {
	if (laser->isActive()) {
		// See if we need to expire the laser.
		if ((!this->isVisible()) || (!this->isActive())) {
			laser->setActive(false);
		}

		if (parent->getVerticalVelocity() > 0) {
			// Jelly moving back up the playfield.
			laser->setActive(false);
		}

		if (laser->isActive()) {
			// The laser is still active.
			Player* player = playState->getPlayer();
			NovaVertex playerPosition = player->getPositionWCS();
			NovaVertex alienPosition = this->getPositionWCS();

			// Update the laser's starting position based on the alien's current position.
			laser->setVertex(LINE_BEGIN, alienPosition.x, alienPosition.y, 0);

			// Always fire straight down.
			double direction = atan2(parent->getVerticalVelocity(), 0);

			// See if we hit the player.
			bool laserHitPlayer = false;

			if (player->isActive()) {
				if (checkLaserHit(alienPosition.x, alienPosition.y, direction, playerPosition.x, playerPosition.y,
						player->getBoundingSphere())) {
					laserHitPlayer = true;

					// We need to work out the distance from the alien to the player (minus the player's bounding sphere)
					// using that distance, and the alien's starting point we need to calculate the laser end point.
					// We cannot use the player's location because it might alter the angle of the laser.
					float distanceToPlayer = (playerPosition - alienPosition).magnitude();

					// Adjust for the player's bounding sphere.
					distanceToPlayer -= player->getBoundingSphere();

					// Update laser end point.
					laser->setVertex(LINE_END, alienPosition.x + (distanceToPlayer * cos(direction)),
							alienPosition.y + (distanceToPlayer * sin(direction)), 0);

					if (player->isShieldActive()) {
						// Create some pretty particles where the laser hits the shield.
						playState->getExplosionController()->createExplosion(laser);
					} else {
						// We killed the Player.
						player->setActive(false);
					}
				}
			}

			if (!laserHitPlayer) {
				// We need to calculate which border we will hit first and then make that our end point.
				auto result = first_border_hit(alienPosition.x, alienPosition.y, direction, VERTICAL_BORDER_POSITION,
						HORIZONTAL_BORDER_POSITION);
				laser->setVertex(LINE_END, result.first, result.second, 0);

				// Create some pretty particles where the laser hits the border.
				playState->getExplosionController()->createExplosion(laser);
			}
		}
	} else {
		// Laser isn't active, see if we are now ready to fire (even if the player's shield is active).
		Player* player = playState->getPlayer();
		if (this->isVisible() && (this->isActive()) && player->isActive()) {
			if (parent->getVerticalVelocity() < 0) {
				NovaVertex alienPosition = this->getPositionWCS();
				NovaVertex playerPosition = player->getPositionWCS();

				// Can't fire when too far away from the player.
				float between = (playerPosition - alienPosition).magnitude();
				if (between < MAXIMUM_TARGET_DISTANCE) {
					// Need to check to see if we are traveling roughly in the correct direction to fire.
					double playerDirection = calculateDirectionFromVelocityComponents((playerPosition.x - alienPosition.x),
											(playerPosition.y - alienPosition.y));
					if (playerDirection < MAXIMUM_TARGET_ANGLE) {
						laser->setActive(true);
					}
				}
			}
		}
	}

	// Need to flash green when heading down the screen to warn the player.
	if (this->isVisible() && (this->isActive())) {
		if (parent->getVerticalVelocity() > 0) {
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
		}
	}
}

bool TentacleNematocyst::checkCollision(Missile* missile) {
	bool collision = false;

	NovaVertex between = (missile->getPositionWCS() - this->getPositionWCS());
	if (between.magnitude() < (missile->getBoundingSphere() + this->getBoundingSphere())) {
		// This segment was hit by the player's missile.
		collision = true;

		if (this->isActive()) {
			damage++;
			if (damage >= MAX_DAMAGE) {
				this->setActive(false);
			}
		}
	}

	return collision;
}

void TentacleNematocyst::draw() {
	// Base processing.
	TentacleSegment::draw();

	// Draw the laser if it is active (implied visibility).
	if (laser->isActive()) {
		laser->draw();
	}
}

