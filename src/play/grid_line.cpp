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

GridLine::GridLine(PlayState* playState) {
	this->playState = playState;
	staticVertices = NULL;
	renderVertices = NULL;
	displayVertices = NULL;
	numVertices = 0;
}

GridLine::~GridLine() {
	if (staticVertices) {
		delete[] staticVertices;
		staticVertices = NULL;
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

void GridLine::init(int numVertices) {
	this->numVertices = numVertices;
	staticVertices = new NovaVertex*[this->numVertices];
	renderVertices = new RenderVertex[this->numVertices];
	displayVertices = new DisplayVertex[this->numVertices];
}

void GridLine::draw() {
	static AstMatrix4x3 objectToCameraMatrix;
    static NovaColor black;

	// Update our composite LCS to CCS matrix that consists of the product of the object to world matrix and the
	// global world to camera matrix.
	objectToCameraMatrix = objectToWorldMatrix * playState->getCamera()->getWorldToCameraMatrix();

	for (int i = 0; i < numVertices; i++) {
		// Reset.
		renderVertices[i].lightSum = lineColor;

        // Transform.
		renderVertices[i] = (*staticVertices[i]) * objectToCameraMatrix;
		
		if (playState->getAlienController()->isWithinBlackHole(renderVertices[i])) {
		    // Apply dark light.
			renderVertices[i].lightSum = black;
		} else {
		    // Apply standard lighting.
		    playState->getExplosionController()->applyLighting(renderVertices[i]);
		}

    	// Project.
    	displayVertices[i].x = horizontalProject(renderVertices[i].x, renderVertices[i].z);
		displayVertices[i].y = verticalProject(renderVertices[i].y, renderVertices[i].z);
	}

	// Loop through the display vertex array and create line segments for each pair of vertices.
	int startVertex = 0;
	int endVertex = 1;
	for (int i = 0; i < (numVertices - 1); i++) {
		glColor3fv(renderVertices[startVertex].lightSum.data);
		glVertex2i(displayVertices[startVertex].x, displayVertices[startVertex].y);

		glColor3fv(renderVertices[endVertex].lightSum.data);
		glVertex2i(displayVertices[endVertex].x, displayVertices[endVertex].y);

		startVertex++;
		endVertex++;
	}
}

