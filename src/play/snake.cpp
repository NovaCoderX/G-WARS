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

#define SEGMENT_SPAWN_INTERVAL 0.3
#define NUMBER_OF_SEGMENTS 21
#define MIN_INITIAL_VELOCITY 6
#define MAX_INITIAL_VELOCITY 9

Snake::Snake(PlayState* playState) : Alien(playState, SNAKE_ALIEN) {
	// The alien container doesn't explode.
	this->setExplosionSize(NO_EXPLOSION);

	NovaColor firstSegmentColor = NovaColor(255, 243, 2);
	NovaColor secondSegmentColor = NovaColor(255, 82, 15);

	// Create the head segment (the first segment).
	headSegment = new SnakeHeadSegment(playState, this, firstSegmentColor);
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
			bodySegment = new SnakeBodySegment(playState, parentSegment, segmentColor);
		} else {
			bodySegment = new SnakeTailSegment(playState, parentSegment, segmentColor);
		}

		// Add to the list of segments.
		bodySegments.push_back(bodySegment);
		segments.push_back(bodySegment);

		// Store.
		parentSegment = bodySegment;
	}

	numSpawnedSegments = 0;
	lastSpawnedTime = 0;
	totalElapsedTime = 0;
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

		// Find out which spawn zone we should be using (based on the players current location).
		if (playState->getPlayAreaController()->isWithinZone(ZoneIndex::LEFT_SPAWN_ZONE, playState->getPlayer())) {
			// Spawn the alien in the right zone.
			this->moveTo(RIGHT_SPAWN_POINT_X, RIGHT_SPAWN_POINT_Y, 0);
			horizontalVelocity = -float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY);

			if (verticalSwitch) {
				verticalVelocity = float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY);
			} else {
				verticalVelocity = -float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY);
			}
		} else {
			// Spawn the alien in the left zone.
			this->moveTo(LEFT_SPAWN_POINT_X, LEFT_SPAWN_POINT_Y, 0);
			horizontalVelocity = float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY);

			if (verticalSwitch) {
				verticalVelocity = float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY);
			} else {
				verticalVelocity = -float_rand(MIN_INITIAL_VELOCITY, MAX_INITIAL_VELOCITY);
			}
		}

		verticalSwitch = (!verticalSwitch);

		// Set up all segments (the segments will be activated later).
		double movementDirection = calculateDirectionFromVelocityComponents(horizontalVelocity, verticalVelocity);
		for (SnakeSegment* segment : segments) {
			segment->setVisible(false);
			segment->moveTo(this->getPositionWCS());
			segment->setVelocityFromComponents(horizontalVelocity, verticalVelocity);
			segment->setFacingTowardsDirection(movementDirection);
		}

		// Reset.
		numSpawnedSegments = 0;
		lastSpawnedTime = 0;
		totalElapsedTime = 0;
	} else {
		for (SnakeSegment* segment : segments) {
			segment->setActive(false);
		}
	}
}

void Snake::update(float elapsedTime) {
	totalElapsedTime += elapsedTime;

	for (int i = 0; i < numSpawnedSegments; i++) {
		segments[i]->update(elapsedTime);
	}

	// If any segments are visible then mark the container as visible (so the draw method is called).
	this->setVisible(false);
	for (SnakeSegment* segment : segments) {
		if (segment->isVisible()) {
			this->setVisible(true);
			break;
		}
	}

	// See if we are ready to spawn the next segment.
	if (numSpawnedSegments < NUMBER_OF_SEGMENTS) {
		if ((totalElapsedTime - lastSpawnedTime) >= SEGMENT_SPAWN_INTERVAL) {
			segments[numSpawnedSegments]->setActive(true);
			numSpawnedSegments++;

			// Store the time of the last spawn.
			lastSpawnedTime = totalElapsedTime;
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
		// Head is dead.
		headSegment->setActive(false);

		// The head is dead, therefore the snake is dead.
		playState->getAlienController()->deactivate(this);
		playState->getPlayer()->increaseScore(this);
	}
}

void Snake::draw() {
	static DisplayVertex displayVertex;

	// Only draw the nodes and connecting line if at least one child or parent node is visible.
	SnakeSegment* parentSegment = headSegment;
	for (int i = 1; i < NUMBER_OF_SEGMENTS; i++) {
		SnakeSegment* childSegment = segments[i];
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

