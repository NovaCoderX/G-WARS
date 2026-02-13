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

#define FIRE_INTERVAL 40
#define FIRE_DURATION 3
#define READY_TO_FIRE_DELAY 3
#define MAXIMUM_TARGET_ANGLE 20
#define MAXIMUM_TARGET_DISTANCE 40
#define SPIN_ANIMATION_SPEED 16.0f

static AstMatrix3x3 rotationMatrix;
static float lastAnimatedTime = 0;

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

// Function to check if the laser hits the target sphere.
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

AttackNeutron::AttackNeutron(PlayState* playState) : AttackAlien(playState, ATTACK_NEUTRON_ALIEN) {
	this->setSpriteDefinition("attack_neutron_base");
	NovaColor defaultColorBase = NovaColor(169, 169, 169);
	defaultColorBase.rebase(80);
	this->setDefaultColor(defaultColorBase);

	center = new Sprite(playState);
	center->setSpriteDefinition("attack_neutron_center");
	defaultColorCenter = NovaColor(169, 169, 169);
	center->setCurrentColor(defaultColorCenter);

	readyToFireColor = NovaColor(201, 255, 4);
	readyToFire = false;
	totalElapsedTime = lastFireTime = readyToFireTime = 0;

	laser = new Laser(playState);
	laser->setColor(readyToFireColor);
}

AttackNeutron::~AttackNeutron() {
	if (center) {
		delete center;
		center = NULL;
	}

	if (laser) {
		delete laser;
		laser = NULL;
	}
}

void AttackNeutron::setActive(bool active) {
	// Base processing.
	AttackAlien::setActive(active);

	if (active) {
		// Reset.
		readyToFire = false;
		laser->setActive(false);
		center->setCurrentColor(defaultColorCenter);
		totalElapsedTime = lastFireTime = readyToFireTime = 0;
	}
}

void AttackNeutron::update(float elapsedTime) {
	totalElapsedTime += elapsedTime;

	// Apply the velocity.
	this->applyVelocity(elapsedTime);

	// See if we have just moved outside the play area.
	bool borderViolated = false;
	if (this->getHorizontalVelocity() > 0) {
		if (!playState->getPlayAreaController()->isWithinPlayArea(RIGHT_BORDER, this)) {
			borderViolated = true;
			this->reverseHorizontalVelocity();
		}
	} else {
		if (!playState->getPlayAreaController()->isWithinPlayArea(LEFT_BORDER, this)) {
			borderViolated = true;
			this->reverseHorizontalVelocity();
		}
	}

	if (this->getVerticalVelocity() > 0) {
		if (!playState->getPlayAreaController()->isWithinPlayArea(TOP_BORDER, this)) {
			borderViolated = true;
			this->reverseVerticalVelocity();
		}
	} else {
		if (!playState->getPlayAreaController()->isWithinPlayArea(BOTTOM_BORDER, this)) {
			borderViolated = true;
			this->reverseVerticalVelocity();
		}
	}

	if (borderViolated) {
		// If the laser is currently active, deactivate it.
		if (laser->isActive()) {
			laser->setActive(false);
			center->setCurrentColor(defaultColorCenter);
		}
	}

	// Mark this object visible/invisible for this frame.
	this->calculateVisibility();

	// If the star is visible...see if the center is also visible.
	if (this->isVisible()) {
		center->moveTo(this->getPositionWCS());
		center->calculateVisibility();

		// Apply animation.
		if (elapsedTime != lastAnimatedTime) {
			// Apply the rotation angle.
			rotationMatrix.MakeZRotation(-(SPIN_ANIMATION_SPEED * elapsedTime));

			for (int i = 0; i < this->definition->getNumVertices(); i++) {
				this->definition->staticVertices[i] = (this->definition->staticVertices[i] * rotationMatrix);
			}

			lastAnimatedTime = elapsedTime;
		}
	} else {
		center->setVisible(false);
	}

	if (laser->isActive()) {
		Player *player = playState->getPlayer();

		// See if we need to expire the laser.
		if ((!center->isVisible()) || ((totalElapsedTime - lastFireTime) > FIRE_DURATION)) {
			laser->setActive(false);
			center->setCurrentColor(defaultColorCenter);
		} else {
			// The laser is still active.
			NovaVertex alienPosition = center->getPositionWCS();

			// Update the laser's starting position based on the alien's current position.
			laser->setVertex(LINE_BEGIN, alienPosition.x, alienPosition.y, 0);

			// Calculate the alien's direction of travel in radians.
			double direction = atan2(verticalVelocity, horizontalVelocity);

			// See if we hit the player.
			bool laserHitPlayer = false;

			if (player->isActive()) {
				NovaVertex playerPosition = player->getPositionWCS();
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
					laser->setVertex(LINE_END, alienPosition.x  + (distanceToPlayer * cos(direction)), alienPosition.y + (distanceToPlayer * sin(direction)), 0);

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
				auto result = first_border_hit(alienPosition.x, alienPosition.y, direction, VERTICAL_BORDER_POSITION, HORIZONTAL_BORDER_POSITION);
				laser->setVertex(LINE_END, result.first, result.second, 0);

				// Create some pretty particles where the laser hits the border.
				playState->getExplosionController()->createExplosion(laser);
			}
		}
	} else {
		// Laser isn't active, see if we are now ready to fire (even if the player's shield is active).
		Player *player = playState->getPlayer();
		if (center->isVisible() && player->isActive()) {
			if (readyToFire) {
				// This is just so that we stay highlighted for a couple of seconds before firing.
				if ((totalElapsedTime - readyToFireTime) >= READY_TO_FIRE_DELAY) {
					NovaVertex alienPosition = center->getPositionWCS();
					NovaVertex playerPosition = player->getPositionWCS();

					// Can't fire when too far away from the player.
					float between = (playerPosition - alienPosition).magnitude();
					if (between < MAXIMUM_TARGET_DISTANCE) {
						// Need to check to see if we are traveling roughly in the correct direction to fire.
						double playerDirection = calculateDirectionFromVelocityComponents((playerPosition.x - alienPosition.x),
								(playerPosition.y - alienPosition.y));

						if (fabs(this->getFacingTowardsDirection() - playerDirection) < MAXIMUM_TARGET_ANGLE) {
							laser->setActive(true);

							// Reset.
							readyToFire = false;

							// Store the last time that we fired.
							lastFireTime = totalElapsedTime;
						}
					}
				}
			} else {
				if ((totalElapsedTime - lastFireTime) >= FIRE_INTERVAL) {
					// Ready to fire again, change color to let the player know.
					readyToFire = true;
					readyToFireTime = totalElapsedTime;
					center->setCurrentColor(readyToFireColor);
				}
			}
		}
	}
}

void AttackNeutron::draw() {
	// Do base processing to draw the base.
	Sprite::draw();

	if (center->isVisible()) {
		center->draw();
	}

	// Draw the laser if it is active (implied visibility).
	if (laser->isActive()) {
		laser->draw();
	}
}

