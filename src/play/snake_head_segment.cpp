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

#define DAMAGE_FACTOR 0.05
#define NUMBER_OF_TEETH_VERTICES 5
#define NUMBER_OF_EYE_VERTICES 5
#define INITIAL_FIRE_DELAY 10
#define FIRE_INTERVAL 10
#define MAXIMUM_TARGET_ANGLE 33

SnakeHeadSegment::SnakeHeadSegment(PlayState* playState, Snake *snake, const NovaColor &highColor) : SnakeSegment(playState) {
	this->snake = snake;
	this->setSpriteDefinition("snake_segment");
	this->highColor = highColor;
	this->lowColor = highColor;
	this->lowColor.rebase(80);
	this->setSpriteColor(highColor);
	this->setExplosionColor(this->getSpriteColor());

	// Create the face data.
	teethData.color = NovaColor(255, 82, 15);
	teethData.staticVertices = new NovaVertex[NUMBER_OF_TEETH_VERTICES];
	teethData.renderVertices = new RenderVertex[NUMBER_OF_TEETH_VERTICES];
	teethData.displayVertices = new DisplayVertex[NUMBER_OF_TEETH_VERTICES];

	teethData.staticVertices[0] = NovaVertex(-10, 20, 0);
	teethData.staticVertices[1] = NovaVertex(-5, 30, 0);
	teethData.staticVertices[2] = NovaVertex(0, 20, 0);
	teethData.staticVertices[3] = NovaVertex(5, 30, 0);
	teethData.staticVertices[4] = NovaVertex(10, 20, 0);

	double scaleFactor = 3;

	for (int i = 0; i < NUMBER_OF_TEETH_VERTICES; i++) {
		teethData.staticVertices[i].x = (teethData.staticVertices[i].x / scaleFactor);
		teethData.staticVertices[i].y = (teethData.staticVertices[i].y / scaleFactor);
		teethData.staticVertices[i].z = (teethData.staticVertices[i].z / scaleFactor);
	}

	leftEyeData.color = NovaColor(255, 82, 15);
	leftEyeData.staticVertices = new NovaVertex[NUMBER_OF_EYE_VERTICES];
	leftEyeData.renderVertices = new RenderVertex[NUMBER_OF_EYE_VERTICES];
	leftEyeData.displayVertices = new DisplayVertex[NUMBER_OF_EYE_VERTICES];

	leftEyeData.staticVertices[0] = NovaVertex(-8, 0, 0);
	leftEyeData.staticVertices[1] = NovaVertex(-13, 5, 0);
	leftEyeData.staticVertices[2] = NovaVertex(-8, 10, 0);
	leftEyeData.staticVertices[3] = NovaVertex(-3, 5, 0);
	leftEyeData.staticVertices[4] = NovaVertex(-8, 0, 0);

	for (int i = 0; i < NUMBER_OF_EYE_VERTICES; i++) {
		leftEyeData.staticVertices[i].x = (leftEyeData.staticVertices[i].x / scaleFactor);
		leftEyeData.staticVertices[i].y = (leftEyeData.staticVertices[i].y / scaleFactor);
		leftEyeData.staticVertices[i].z = (leftEyeData.staticVertices[i].z / scaleFactor);
	}

	rightEyeData.color = NovaColor(255, 82, 15);
	rightEyeData.staticVertices = new NovaVertex[NUMBER_OF_EYE_VERTICES];
	rightEyeData.renderVertices = new RenderVertex[NUMBER_OF_EYE_VERTICES];
	rightEyeData.displayVertices = new DisplayVertex[NUMBER_OF_EYE_VERTICES];

	rightEyeData.staticVertices[0] = NovaVertex(8, 0, 0);
	rightEyeData.staticVertices[1] = NovaVertex(13, 5, 0);
	rightEyeData.staticVertices[2] = NovaVertex(8, 10, 0);
	rightEyeData.staticVertices[3] = NovaVertex(3, 5, 0);
	rightEyeData.staticVertices[4] = NovaVertex(8, 0, 0);

	for (int i = 0; i < NUMBER_OF_EYE_VERTICES; i++) {
		rightEyeData.staticVertices[i].x = (rightEyeData.staticVertices[i].x / scaleFactor);
		rightEyeData.staticVertices[i].y = (rightEyeData.staticVertices[i].y / scaleFactor);
		rightEyeData.staticVertices[i].z = (rightEyeData.staticVertices[i].z / scaleFactor);
	}

	totalElapsedTime = lastFireTime = 0;
}

SnakeHeadSegment::~SnakeHeadSegment() {
	if (teethData.staticVertices) {
		delete[] teethData.staticVertices;
		teethData.staticVertices = NULL;
	}

	if (teethData.renderVertices) {
		delete[] teethData.renderVertices;
		teethData.renderVertices = NULL;
	}

	if (teethData.displayVertices) {
		delete[] teethData.displayVertices;
		teethData.displayVertices = NULL;
	}

	if (leftEyeData.staticVertices) {
		delete[] leftEyeData.staticVertices;
		leftEyeData.staticVertices = NULL;
	}

	if (leftEyeData.renderVertices) {
		delete[] leftEyeData.renderVertices;
		leftEyeData.renderVertices = NULL;
	}

	if (leftEyeData.displayVertices) {
		delete[] leftEyeData.displayVertices;
		leftEyeData.displayVertices = NULL;
	}

	if (rightEyeData.staticVertices) {
		delete[] rightEyeData.staticVertices;
		rightEyeData.staticVertices = NULL;
	}

	if (rightEyeData.renderVertices) {
		delete[] rightEyeData.renderVertices;
		rightEyeData.renderVertices = NULL;
	}

	if (rightEyeData.displayVertices) {
		delete[] rightEyeData.displayVertices;
		rightEyeData.displayVertices = NULL;
	}
}

void SnakeHeadSegment::setActive(bool active) {
	if (active) {
		// Reset.
		this->setSpriteColor(highColor);
	}

	// Base processing.
	SnakeSegment::setActive(active);
}

void SnakeHeadSegment::update(float elapsedTime) {
	static NovaVertex playerPosition;
	static NovaVertex alienPosition;

	totalElapsedTime += elapsedTime;

	// Apply the velocity.
	this->applyVelocity(elapsedTime);

	bool borderViolated = false;

	// See if we have just moved outside the play area.
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
		// Rotate in the direction of travel.
		double movementDirection = calculateDirectionFromVelocityComponents(this->getHorizontalVelocity(), this->getVerticalVelocity());
		this->setFacingTowardsDirection(movementDirection);
	}

	// Mark this object visible/invisible for this frame.
	this->calculateVisibility();

	// See if we are now ready to fire a missile.
	Player *player = playState->getPlayer();
	if (player->isActive() && (this->isVisible())) {
		if (totalElapsedTime > INITIAL_FIRE_DELAY) {
			if ((totalElapsedTime - lastFireTime) >= FIRE_INTERVAL) {
				playerPosition = player->getPositionWCS();
				alienPosition = this->getPositionWCS();

				double playerDirection = calculateDirectionFromVelocityComponents((playerPosition.x - alienPosition.x),
						(playerPosition.y - alienPosition.y));

				// Need to check to see if we are traveling roughly in the correct direction to fire.
				if (fabs(this->getFacingTowardsDirection() - playerDirection) < MAXIMUM_TARGET_ANGLE) {
					playState->getMissileController()->launchMissile(this, this->getFacingTowardsDirection(), this->getAnchorPointWCS(TOP_ANCHOR));

					// Store the last time that we fired.
					lastFireTime = totalElapsedTime;
				}
			}
		}
	}
}

void SnakeHeadSegment::youHit(Player *player) {
	// This segment was hit by the player, optionally destroy the player.
	player->youHit(this);
}

void SnakeHeadSegment::youHit(Missile *missile) {
	// This segment was hit by the player's missile.
	if (snake->isHeadNodeVulnerable()) {
		if (this->spriteColor.fade(DAMAGE_FACTOR, lowColor)) {
			// The head is dead, therefore the snake is dead.
			playState->getAlienController()->deactivate(snake);

			// This alien was destroyed by a player's missile.
			playState->getPlayer()->increaseScore(snake);
		}
	}

	// Missile has also been destroyed.
	playState->getMissileController()->deactivate(missile);
}

void SnakeHeadSegment::draw() {
	static AstMatrix4x3 objectToCameraMatrix;

	// Update our composite LCS to CCS matrix that consists of the product of the object to world matrix and the
	// global world to camera matrix.
	objectToCameraMatrix = objectToWorldMatrix * playState->getCamera()->getWorldToCameraMatrix();

	// Base processing.
	SnakeSegment::draw();

	// Then draw the teeth.
	for (int i = 0; i < NUMBER_OF_TEETH_VERTICES; i++) {
		// Transform.
		teethData.renderVertices[i] = teethData.staticVertices[i] * objectToCameraMatrix;

		// Project.
		teethData.displayVertices[i].x = horizontalProject(teethData.renderVertices[i].x,
				teethData.renderVertices[i].z);
		teethData.displayVertices[i].y = verticalProject(teethData.renderVertices[i].y,
				teethData.renderVertices[i].z);
	}

	// Loop through the display vertex array and create line segments for each pair of vertices.
	int startVertex = 0;
	int endVertex = 1;

	glColor3fv(teethData.color.data);

	for (int i = 0; i < (NUMBER_OF_TEETH_VERTICES - 1); i++) {
		glVertex2i(teethData.displayVertices[startVertex].x, teethData.displayVertices[startVertex].y);
		glVertex2i(teethData.displayVertices[endVertex].x, teethData.displayVertices[endVertex].y);
		startVertex++;
		endVertex++;
	}

	// And then the eyes.
	for (int i = 0; i < NUMBER_OF_EYE_VERTICES; i++) {
		// Transform.
		leftEyeData.renderVertices[i] = leftEyeData.staticVertices[i] * objectToCameraMatrix;

		// Project.
		leftEyeData.displayVertices[i].x = horizontalProject(leftEyeData.renderVertices[i].x,
				leftEyeData.renderVertices[i].z);
		leftEyeData.displayVertices[i].y = verticalProject(leftEyeData.renderVertices[i].y,
				leftEyeData.renderVertices[i].z);
	}

	// Reset.
	startVertex = 0;
	endVertex = 1;
	glColor3fv(leftEyeData.color.data);

	for (int i = 0; i < (NUMBER_OF_EYE_VERTICES - 1); i++) {
		glVertex2i(leftEyeData.displayVertices[startVertex].x, leftEyeData.displayVertices[startVertex].y);
		glVertex2i(leftEyeData.displayVertices[endVertex].x, leftEyeData.displayVertices[endVertex].y);
		startVertex++;
		endVertex++;
	}

	for (int i = 0; i < NUMBER_OF_EYE_VERTICES; i++) {
		// Transform.
		rightEyeData.renderVertices[i] = rightEyeData.staticVertices[i] * objectToCameraMatrix;

		// Project.
		rightEyeData.displayVertices[i].x = horizontalProject(rightEyeData.renderVertices[i].x,
				rightEyeData.renderVertices[i].z);
		rightEyeData.displayVertices[i].y = verticalProject(rightEyeData.renderVertices[i].y,
				rightEyeData.renderVertices[i].z);
	}

	// Reset.
	startVertex = 0;
	endVertex = 1;
	glColor3fv(rightEyeData.color.data);

	for (int i = 0; i < (NUMBER_OF_EYE_VERTICES - 1); i++) {
		glVertex2i(rightEyeData.displayVertices[startVertex].x, rightEyeData.displayVertices[startVertex].y);
		glVertex2i(rightEyeData.displayVertices[endVertex].x, rightEyeData.displayVertices[endVertex].y);
		startVertex++;
		endVertex++;
	}
}
