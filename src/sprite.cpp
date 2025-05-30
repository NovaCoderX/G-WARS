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

Sprite::Sprite(GameState *gameState) {
	this->gameState = gameState;
	definition = NULL;
	renderVertices = NULL;
	displayVertices = NULL;
	lines = NULL;
	spriteColor = explosionColor = NovaColor(255, 255, 255);
	facingTowardsDirection = 0;
	horizontalVelocity = verticalVelocity = 0;
}

Sprite::~Sprite() {
	if (lines) {
		delete[] lines;
		lines = NULL;
	}

	if (renderVertices) {
		delete[] renderVertices;
		renderVertices = NULL;
	}

	if (displayVertices) {
		delete[] displayVertices;
		displayVertices = NULL;
	}
}

void Sprite::setSpriteDefinition(SpriteDefinition *definition) {
	if (!definition) {
		fatalError("Definition is invalid\n");
	}

	if (!definition->getNumVertices()) {
		fatalError("Definition is invalid\n");
	}

	if (!definition->getNumLines()) {
		fatalError("Definition is invalid\n");
	}

	this->definition = definition;

	if (renderVertices) {
		delete[] renderVertices;
		renderVertices = NULL;
	}

	renderVertices = new RenderVertex[definition->getNumVertices()];
	if (!renderVertices) {
		fatalError("Could not allocate scratch_vertex_array");
	}

	if (displayVertices) {
		delete[] displayVertices;
		displayVertices = NULL;
	}

	displayVertices = new DisplayVertex[definition->getNumVertices()];
	if (!displayVertices) {
		fatalError("Could not allocate displayVertices");
	}

	if (lines) {
		delete[] lines;
		lines = NULL;
	}

	lines = new NovaLine[definition->getNumLines()];
	if (!lines) {
		fatalError("Could not allocate lines");
	}

	// The  use shared static vertices.
	for (int index = 0; index < definition->getNumLines(); index++) {
		int startVertex = definition->lineVertexMap[index].vertexIndex[BEGIN];
		lines[index].staticVertices[BEGIN] = &definition->staticVertices[startVertex];
		lines[index].renderVertices[BEGIN] = &renderVertices[startVertex];
		lines[index].displayVertices[BEGIN] = &displayVertices[startVertex];

		int endVertex = definition->lineVertexMap[index].vertexIndex[END];
		lines[index].staticVertices[END] = &definition->staticVertices[endVertex];
		lines[index].renderVertices[END] = &renderVertices[endVertex];
		lines[index].displayVertices[END] = &displayVertices[endVertex];
	}
}

void Sprite::setSpriteDefinition(const char *name) {
	this->setSpriteDefinition(g_worldManager->getSpriteDefinition(name));
}

void Sprite::setVelocityFromDirection(double direction, double totalVelocity) {
	if (totalVelocity > 0) {
		// Adjust angle to make up (positive y-axis)
		double radians = (direction + 90) * (M_PI / 180.0);
		horizontalVelocity = totalVelocity * cos(radians);
		verticalVelocity = totalVelocity * sin(radians);

		// Correct for small floating-point precision errors
		if (std::abs(horizontalVelocity) < 1e-6) {
			horizontalVelocity = 0.0;
		}

		if (std::abs(verticalVelocity) < 1e-6) {
			verticalVelocity = 0.0;
		}
	} else {
		// We have stopped moving completely.
		horizontalVelocity = 0;
		verticalVelocity = 0;
	}
}

void Sprite::applyVelocity(float elapsedTime) {
	static NovaVertex currentPosition;

	if ((horizontalVelocity != 0) || (verticalVelocity != 0)) {
		currentPosition = this->getPositionWCS();
		currentPosition.x += (horizontalVelocity * elapsedTime);
		currentPosition.y += (verticalVelocity * elapsedTime);
		this->moveTo(currentPosition);
	}
}

void Sprite::calculateVisibility() {
	// Update for derived class usage.
	positionCCS = gameState->getCamera()->getPositionCCS(this);

	// If the object's origin is within the view port then it's definitely visible.
	if (gameState->getCamera()->checkProjectedPoint(positionCCS)) {
		visible = true;
	} else {
		// Take the objects extents into account.
		NovaVertex topR = NovaVertex(getBoundingSphere() + positionCCS.x, getBoundingSphere() + positionCCS.y, positionCCS.z);
		NovaVertex botL = NovaVertex(-getBoundingSphere() + positionCCS.x, -getBoundingSphere() + positionCCS.y, positionCCS.z);
		visible = gameState->getCamera()->checkProjectedPoints(topR, botL);
	}
}

void Sprite::draw() {
	static AstMatrix4x3 objectToCameraMatrix;

	// Reset all (shared) scratch data used by this object.
	for (int i = 0; i < definition->getNumVertices(); i++) {
		renderVertices[i].transformedAndProjected = false;
	}

	// Update our composite LCS to CCS matrix that consists of the product of the object to world matrix and the
	// global world to camera matrix.
	objectToCameraMatrix = objectToWorldMatrix * gameState->getCamera()->getWorldToCameraMatrix();

	// All  have the same color.
	glColor3fv(spriteColor.data);

	for (int i = 0; i < definition->getNumLines(); i++) {
		NovaLine *line = &lines[i];

		// We only transform (LCS to CCS) and project (CCS to screen coordinates) a single (shared) static vertex once per frame.
		if (!line->renderVertices[BEGIN]->transformedAndProjected) {
			// Transform.
			(*line->renderVertices[BEGIN]) = (*line->staticVertices[BEGIN]) * objectToCameraMatrix;

			// Project.
			line->displayVertices[BEGIN]->x = horizontalProject(line->renderVertices[BEGIN]->x,
					line->renderVertices[BEGIN]->z);
			line->displayVertices[BEGIN]->y = verticalProject(line->renderVertices[BEGIN]->y,
					line->renderVertices[BEGIN]->z);

			line->renderVertices[BEGIN]->transformedAndProjected = true;
		}

		if (!line->renderVertices[END]->transformedAndProjected) {
			// Transform.
			(*line->renderVertices[END]) = (*line->staticVertices[END]) * objectToCameraMatrix;

			// Project.
			line->displayVertices[END]->x = horizontalProject(line->renderVertices[END]->x,
					line->renderVertices[END]->z);
			line->displayVertices[END]->y = verticalProject(line->renderVertices[END]->y,
					line->renderVertices[END]->z);

			line->renderVertices[END]->transformedAndProjected = true;
		}

		glVertex2i(line->displayVertices[BEGIN]->x, line->displayVertices[BEGIN]->y);
		glVertex2i(line->displayVertices[END]->x, line->displayVertices[END]->y);
	}
}

void Sprite::setFacingTowardsDirection(double direction) {
	// See if we are changing degrees.
	if (direction != facingTowardsDirection) {
		// Apply the difference in degrees.
		rotateAroundZ(direction - facingTowardsDirection);

		// Store.
		facingTowardsDirection = direction;
	}
}

