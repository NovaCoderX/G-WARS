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

CharacterSprite::CharacterSprite(GameState* gameState) : Sprite(gameState) {
	transformed = false;
}

void CharacterSprite::init(SpriteDefinition *definition, const NovaColor &color) {
	this->setSpriteDefinition(definition);
	spriteColor = color;
}

void CharacterSprite::transform() {
	static AstMatrix4x3 objectToCameraMatrix;
	static AstMatrix4x3 worldToCameraMatrix;

	// Reset all (shared) scratch data used by this object.
	for (int i = 0; i < definition->getNumVertices(); i++) {
		renderVertices[i].transformedAndProjected = false;
	}

	if (!transformed) {
		// We want to store the original world to camera matrix (before the camera is moved during gameplay).
		worldToCameraMatrix = gameState->getCamera()->getWorldToCameraMatrix();
		transformed = true;
	}

	// Update our composite LCS to CCS matrix that consists of the product of the object to world matrix and the
	// global world to camera matrix.
	objectToCameraMatrix = objectToWorldMatrix * worldToCameraMatrix;

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
	}
}

void CharacterSprite::draw() {
	// All lines have the same color.
	glColor3fv(spriteColor.data);

	for (int i = 0; i < definition->getNumLines(); i++) {
		NovaLine *line = &lines[i];

		glVertex2i(line->displayVertices[BEGIN]->x, line->displayVertices[BEGIN]->y);
		glVertex2i(line->displayVertices[END]->x, line->displayVertices[END]->y);
	}
}

