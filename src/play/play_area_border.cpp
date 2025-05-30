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

PlayAreaBorder::PlayAreaBorder(PlayState* playState) {
	this->playState = playState;
}

void PlayAreaBorder::draw() {
	static AstMatrix4x3 objectToCameraMatrix;

	// Update our composite LCS to CCS matrix that consists of the product of the object to world matrix and the
	// global world to camera matrix.
	objectToCameraMatrix = objectToWorldMatrix * playState->getCamera()->getWorldToCameraMatrix();

	renderVertices[BEGIN] = (*staticVertices[BEGIN]) * objectToCameraMatrix;

	displayVertices[BEGIN].x = horizontalProject(renderVertices[BEGIN].x, renderVertices[BEGIN].z);
	displayVertices[BEGIN].y = verticalProject(renderVertices[BEGIN].y, renderVertices[BEGIN].z);;

	renderVertices[END] = (*staticVertices[END]) * objectToCameraMatrix;

	displayVertices[END].x = horizontalProject(renderVertices[END].x, renderVertices[END].z);
	displayVertices[END].y = verticalProject(renderVertices[END].y, renderVertices[END].z);

	glColor3fv(lineColors[BEGIN]->data);
	glVertex2i(displayVertices[BEGIN].x, displayVertices[BEGIN].y);

	glColor3fv(lineColors[END]->data);
	glVertex2i(displayVertices[END].x, displayVertices[END].y);
}

