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

#define NUMBER_OF_TAIL_VERTICES 4

SnakeTailSegment::SnakeTailSegment(PlayState* playState, Sprite* parent, const NovaColor& color, bool explosive) :
		SnakeBodySegment(playState, parent, color, explosive) {
	// Create the tail data.
	tailData.color = NovaColor(255, 82, 15);
	tailData.staticVertices = new NovaVertex[NUMBER_OF_TAIL_VERTICES];
	tailData.renderVertices = new RenderVertex[NUMBER_OF_TAIL_VERTICES];
	tailData.displayVertices = new DisplayVertex[NUMBER_OF_TAIL_VERTICES];

	tailData.staticVertices[0] = NovaVertex(0, -30, 0);
	tailData.staticVertices[1] = NovaVertex(-10, -20, 0);
	tailData.staticVertices[2] = NovaVertex(10, -20, 0);
	tailData.staticVertices[3] = NovaVertex(0, -30, 0);

	double scaleFactor = 3;

	for (int i = 0; i < NUMBER_OF_TAIL_VERTICES; i++) {
		tailData.staticVertices[i].x = (tailData.staticVertices[i].x / scaleFactor);
		tailData.staticVertices[i].y = (tailData.staticVertices[i].y / scaleFactor);
		tailData.staticVertices[i].z = (tailData.staticVertices[i].z / scaleFactor);
	}
}

SnakeTailSegment::~SnakeTailSegment() {
	if (tailData.staticVertices) {
		delete[] tailData.staticVertices;
		tailData.staticVertices = NULL;
	}

	if (tailData.renderVertices) {
		delete[] tailData.renderVertices;
		tailData.renderVertices = NULL;
	}

	if (tailData.displayVertices) {
		delete[] tailData.displayVertices;
		tailData.displayVertices = NULL;
	}
}

void SnakeTailSegment::draw() {
	static AstMatrix4x3 objectToCameraMatrix;

	// Update our composite LCS to CCS matrix that consists of the product of the object to world matrix and the
	// global world to camera matrix.
	objectToCameraMatrix = objectToWorldMatrix * playState->getCamera()->getWorldToCameraMatrix();

	// Base processing.
	SnakeBodySegment::draw();

	// Then draw the tail.
	for (int i = 0; i < NUMBER_OF_TAIL_VERTICES; i++) {
		// Transform.
		tailData.renderVertices[i] = tailData.staticVertices[i] * objectToCameraMatrix;

		// Project.
		tailData.displayVertices[i].x = horizontalProject(tailData.renderVertices[i].x, tailData.renderVertices[i].z);
		tailData.displayVertices[i].y = verticalProject(tailData.renderVertices[i].y, tailData.renderVertices[i].z);
	}

	// Loop through the display vertex array and create line segments for each pair of vertices.
	int startVertex = 0;
	int endVertex = 1;

	glColor3fv(tailData.color.data);

	for (int i = 0; i < (NUMBER_OF_TAIL_VERTICES - 1); i++) {
		glVertex2i(tailData.displayVertices[startVertex].x, tailData.displayVertices[startVertex].y);
		glVertex2i(tailData.displayVertices[endVertex].x, tailData.displayVertices[endVertex].y);
		startVertex++;
		endVertex++;
	}
}
