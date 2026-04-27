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

#define NUMBER_OF_SEGMENTS 21
#define MIN_INITIAL_VELOCITY 6
#define MAX_INITIAL_VELOCITY 9

Snake::Snake(PlayState* playState) : Alien(playState, BOSS_ALIEN) {
	// The alien container doesn't explode.
	this->setExplosionSize(NO_EXPLOSION);

	NovaColor firstSegmentColor = NovaColor(255, 243, 2);
	NovaColor secondSegmentColor = NovaColor(255, 82, 15);

	// Create the head segment (the first segment).
	headSegment = new SnakeHeadSegment(playState, firstSegmentColor);
	segments.push_back(headSegment);

	// The first body segment's parent will be the head.
	SnakeSegment* parentSegment = headSegment;

	// Create the body segments.
	for (int i = 1; i < NUMBER_OF_SEGMENTS; i++) {
		// Swap colors.
		NovaColor segmentColor = secondSegmentColor;
		if (i % 2 == 0) {
			segmentColor = firstSegmentColor;
		}

		SnakeBodySegment* bodySegment = NULL;
		if (i < (NUMBER_OF_SEGMENTS - 1)) {
			bodySegment = new SnakeBodySegment(parentSegment, segmentColor);
		} else {
			bodySegment = new SnakeTailSegment(parentSegment, segmentColor);
		}

		// Add to the list of segments.
		bodySegments.push_back(bodySegment);
		segments.push_back(bodySegment);

		// Store.
		parentSegment = bodySegment;
	}
}

Snake::~Snake() {
	segments.clear();

	if (headSegment) {
		delete headSegment;
		headSegment = NULL;
	}

	for (SnakeBodySegment* segment : bodySegments) {
		delete segment;
	}

	bodySegments.clear();
}

void Snake::setActive(bool active) {
	static bool verticalSwitch = true;

	// Base processing.
	Alien::setActive(active);

	if (active) {
		// Make some sound.
		g_worldManager->startSound(ENEMY_SPAWN_BOSS);

		// Set initial position, direction and velocity of the new alien.
		this->moveTo(0, 0, 0);
		this->setVelocityFromComponents(0, 0);

		// Set up the head segment.
		headSegment->setActive(true);

		// Find out which spawn zone we should be using (based on the players current location).
		if (playState->getPlayAreaController()->isWithinZone(ZoneIndex::LEFT_SPAWN_ZONE, playState->getPlayer())) {
			// Spawn the alien in the right zone.
			headSegment->moveTo(RIGHT_SPAWN_POINT_X, RIGHT_SPAWN_POINT_Y, 0);

			if (verticalSwitch) {
				headSegment->setVelocityFromComponents(-float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY), float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY));
			} else {
				headSegment->setVelocityFromComponents(-float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY), -float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY));
			}
		} else {
			// Spawn the alien in the left zone.
			headSegment->moveTo(LEFT_SPAWN_POINT_X, LEFT_SPAWN_POINT_Y, 0);

			if (verticalSwitch) {
				headSegment->setVelocityFromComponents(float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY), float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY));
			} else {
				headSegment->setVelocityFromComponents(float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY), -float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY));
			}
		}

		headSegment->setFacingTowardsDirection(calculateDirectionFromVelocityComponents(headSegment->getHorizontalVelocity(), headSegment->getVerticalVelocity()));
		verticalSwitch = (!verticalSwitch);

		// Set up all the body segments.
		for (SnakeBodySegment* bodySegment : bodySegments) {
			bodySegment->setActive(true);
			bodySegment->moveTo(headSegment->getPositionWCS());
			bodySegment->setVelocityFromComponents(0, 0);
			bodySegment->setFacingTowardsDirection(headSegment->getFacingTowardsDirection());
		}
	}
}

void Snake::update(float elapsedTime) {
	for (SnakeSegment* segment : segments) {
		segment->update(elapsedTime);
	}

	// If any segments are visible then mark the container as visible (so the draw method is called).
	this->setVisible(false);
	for (SnakeSegment* segment : segments) {
		if (segment->isVisible()) {
			this->setVisible(true);
			break;
		}
	}
}

bool Snake::checkCollision(Player* player) {
	bool collision = false;

	// See if this alien has just hit the player.
	for (SnakeSegment* segment : segments) {
		if (segment->isVisible()) {
			if (segment->checkCollision(player)) {
				// This alien was hit by the player.
				collision = true;

				// Optionally destroy the player.
				player->youHit(this);
				break;
			}
		}
	}

	return collision;
}

bool Snake::checkCollision(Missile* missile) {
	bool collision = false;

	// See if this alien has just hit this missile, first check the body segments.
	for (SnakeBodySegment* segment : bodySegments) {
		if (segment->isVisible()) {
			collision = segment->checkCollision(missile);
			if (collision) {
				break;
			}
		}
	}

	if (collision) {
		if (!headSegment->isVulnerable()) {
			// See if the head is vulnerable as a result of this body collision.
			bool vulnerable = true;
			for (SnakeBodySegment* segment : bodySegments) {
				if (segment->isActive()) {
					vulnerable = false;
					break;
				}
			}

			if (vulnerable) {
				// The head can now be disabled.
				headSegment->setVulnerable(true);
			}
		}
	}

	if (!collision) {
		// If no body collision, check the head.
		if (headSegment->isVisible()) {
			collision = headSegment->checkCollision(missile);
		}
	}

	if (collision) {
		if (!headSegment->isActive()) {
			// The head is dead, therefore the snake is dead.
			playState->getAlienController()->deactivate(this);
			playState->getPlayer()->increaseScore(this);
		}

		// Missile has been destroyed.
		playState->getMissileController()->deactivate(missile);
	}

	return collision;
}

void Snake::smartBombNotification() {
	if (!headSegment->isVulnerable()) {
		for (SnakeBodySegment* segment : bodySegments) {
			if (segment->isActive()) {
				segment->setActive(false);
				break;
			}
		}

		// See if the head is vulnerable as a result of the smart bomb.
		bool vulnerable = true;
		for (SnakeBodySegment* segment : bodySegments) {
			if (segment->isActive()) {
				vulnerable = false;
				break;
			}
		}

		if (vulnerable) {
			// The head can now be destroyed.
			headSegment->setVulnerable(true);
		}
	} else {
		// Head is now dead.
		headSegment->setActive(false);

		// The head is dead, therefore the snake is dead.
		playState->getAlienController()->deactivate(this);
	}
}

void Snake::draw() {
	static DisplayVertex displayVertex;

	// Only draw the nodes and connecting line if the child or parent node is visible.
	SnakeSegment* parentSegment = headSegment;
	for (SnakeBodySegment* bodySegment : bodySegments) {
		SnakeSegment* childSegment = bodySegment;
		if (parentSegment->isVisible() || childSegment->isVisible()) {
			// Draw the line.
			NovaVertex start = childSegment->getAnchorPointCCS(SnakeSegment::BOTTOM_VERTEX_ANCHOR);
			displayVertex.x = horizontalProject(start.x, start.z);
			displayVertex.y = verticalProject(start.y, start.z);
			glColor3fv(childSegment->getCurrentColor().data);
			glVertex2i(displayVertex.x, displayVertex.y);

			NovaVertex finish = parentSegment->getAnchorPointCCS(SnakeSegment::BOTTOM_VERTEX_ANCHOR);
			displayVertex.x = horizontalProject(finish.x, finish.z);
			displayVertex.y = verticalProject(finish.y, finish.z);
			glColor3fv(parentSegment->getCurrentColor().data);
			glVertex2i(displayVertex.x, displayVertex.y);

			// Draw the segments.
			if (parentSegment->isVisible()) {
				parentSegment->draw();
			}

			if (childSegment->isVisible()) {
				childSegment->draw();
			}
		}

		// Move down one node.
		parentSegment = childSegment;
	}
}

