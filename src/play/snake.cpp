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
#define NUMBER_OF_BODY_SEGMENTS (NUMBER_OF_SEGMENTS - 2)

Snake::Snake(PlayState* playState) : Alien(playState) {
	this->alienType = SNAKE;
	NovaColor headColor = NovaColor(255, 243, 2);
	NovaColor bodyOneColor = NovaColor(255, 82, 15);
	NovaColor bodyTwoColor = NovaColor(255, 243, 2);

	// Create the head and body segments.
	segments = new SnakeSegment*[NUMBER_OF_SEGMENTS];
	headSegment = new SnakeHeadSegment(playState, this, headColor);
	segments[0] = headSegment;

	bool colorSwitch = true;
	for (int i = 0; i < NUMBER_OF_BODY_SEGMENTS; i++) {
		if (colorSwitch) {
			bodySegments.push_back(new SnakeBodySegment(playState, bodyOneColor));
		}  else {
			bodySegments.push_back(new SnakeBodySegment(playState, bodyTwoColor));
		}

		colorSwitch = (!colorSwitch);

		// Setup the parent references.
		if (i == 0) {
			bodySegments[i]->setParent(headSegment);
		} else {
			bodySegments[i]->setParent(bodySegments[i - 1]);
		}

		// Add to the list of segments.
		segments[i + 1] = bodySegments[i];
	}

	if (colorSwitch) {
		tailSegment = new SnakeTailSegment(playState, bodyOneColor);
	}  else {
		tailSegment = new SnakeTailSegment(playState, bodyTwoColor);
	}

	// Setup the parent references.
	tailSegment->setParent(bodySegments[NUMBER_OF_BODY_SEGMENTS - 1]);

	// Add to the list of segments.
	segments[NUMBER_OF_SEGMENTS - 1] = tailSegment;

	numSpawnedSegments = 0;
	lastSpawnedTime = 0;
	totalElapsedTime = 0;
}

Snake::~Snake() {
	if (headSegment) {
		delete headSegment;
		headSegment = NULL;
	}

	for (SnakeBodySegment* segment : bodySegments) {
		delete segment;
	}

	bodySegments.clear();

	if (tailSegment) {
		delete tailSegment;
		tailSegment = NULL;
	}

	if (segments) {
		delete[] segments;
		segments = NULL;
	}
}

void Snake::setActive(bool active) {
	if (active) {
	    // Make some sound.
		g_worldManager->startSound(ENEMY_SPAWN_SNAKE);
	    
		// Setup all segments.
		double movementDirection = calculateDirectionFromVelocityComponents(this->getHorizontalVelocity(), this->getVerticalVelocity());

		for (int i = 0; i < NUMBER_OF_SEGMENTS; i++) {
			segments[i]->moveTo(this->getPositionWCS());
			segments[i]->setVelocityFromComponents(this->getHorizontalVelocity(), this->getVerticalVelocity());
			segments[i]->setFacingTowardsDirection(movementDirection);
		}

		// Reset.
		numSpawnedSegments = 0;
		lastSpawnedTime = 0;
		totalElapsedTime = 0;
	} else if (this->isActive()) {
		for (int i = 0; i < NUMBER_OF_SEGMENTS; i++) {
			// This will create an explosion for each segment.
			segments[i]->setActive(false);
		}
	}

	// Base processing - Note, we do not want to use the base Alien logic.
	Sprite::setActive(active);
}

void Snake::update(float elapsedTime) {
	totalElapsedTime += elapsedTime;

	for (int i = 0; i < NUMBER_OF_SEGMENTS; i++) {
		if (segments[i]->isActive()) {
			segments[i]->update(elapsedTime);
		}
	}

	// If any segments are visible then mark the container as visible (so the draw method is called).
	this->setVisible(false);
	for (int i = 0; i < NUMBER_OF_SEGMENTS; i++) {
		if (segments[i]->isVisible()) {
			this->setVisible(true);
			break;
		}
	}

	// See if we need to spawn the next segment.
	if (numSpawnedSegments < NUMBER_OF_SEGMENTS) {
		if ((totalElapsedTime - lastSpawnedTime) >= SEGMENT_SPAWN_INTERVAL) {
			segments[numSpawnedSegments]->setActive(true);
			numSpawnedSegments++;

			// Store the time of the last spawn.
			lastSpawnedTime = totalElapsedTime;
		}
	}
}

void Snake::checkCollision(Player *player) {
	// See if this alien has just hit the player.
	for (int i = 0; i < NUMBER_OF_SEGMENTS; i++) {
		if (segments[i]->isVisible()) {
			NovaVertex between = (player->getPositionWCS() - segments[i]->getPositionWCS());
			if (between.magnitude() < (player->getBoundingSphere() + segments[i]->getBoundingSphere())) {
				// A collision occurred, conditionally destroy the alien and player.
				segments[i]->youHit(player);
			}
		}
	}
}

void Snake::checkCollision(Missile *missile) {
	// See if this alien has just hit this missile.
	for (int i = 0; i < NUMBER_OF_SEGMENTS; i++) {
		if (segments[i]->isVisible()) {
			NovaVertex between = (missile->getPositionWCS() - segments[i]->getPositionWCS());
			if (between.magnitude() < (missile->getBoundingSphere() + segments[i]->getBoundingSphere())) {
				// A collision occurred, conditionally destroy the alien and missile.
				segments[i]->youHit(missile);
			}
		}
	}
}

void Snake::draw() {
	static DisplayVertex displayVertex;

	// Only draw the nodes and connecting line if at least one child or parent node is visible.
	SnakeSegment *parent = segments[0];
	for (int i = 1; i < NUMBER_OF_SEGMENTS; i++) {
		SnakeSegment *child = segments[i];
		if (parent->isVisible() || child->isVisible()) {
			// Draw the line.
			NovaVertex start = child->getAnchorPointCCS(TOP_ANCHOR);
			displayVertex.x = horizontalProject(start.x, start.z);
			displayVertex.y = verticalProject(start.y, start.z);
			glColor3fv(child->getSpriteColor().data);
			glVertex2i(displayVertex.x, displayVertex.y);

			NovaVertex finish = parent->getAnchorPointCCS(BOTTOM_ANCHOR);
			displayVertex.x = horizontalProject(finish.x, finish.z);
			displayVertex.y = verticalProject(finish.y, finish.z);
			glColor3fv(parent->getSpriteColor().data);
			glVertex2i(displayVertex.x, displayVertex.y);

			// Draw the segments.
			if (parent->isVisible()) {
				parent->draw();
			}

			if (child->isVisible()) {
				child->draw();
			}
		}

		// Move down one node.
		parent = child;
	}
}

bool Snake::isHeadNodeVulnerable() {
	bool vulnerable = true;

	// See if all of the spawned body segments have been disabled.
	for (int i = 0; i < NUMBER_OF_BODY_SEGMENTS; i++) {
		if (bodySegments[i]->isActive() && (!bodySegments[i]->isDisabled())) {
			vulnerable = false;
			break;
		}
	}

	return vulnerable;
}
