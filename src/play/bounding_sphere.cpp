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

#define NUMBER_OF_POINTS 8

BoundingSphere::BoundingSphere(PlayState* playState, Sprite* parent, int anchorIndex, float radius) : AlienComponent(playState, parent) {
	this->anchorIndex = anchorIndex;

	// Create the definition.
	sphereDefinition = new SpriteDefinition();

	// Create the vertex data.
	float quadrant = (radius / 2);
	sphereDefinition->allocateVertices(NUMBER_OF_POINTS);
	sphereDefinition->setVertex(0, -quadrant, radius, 0);
	sphereDefinition->setVertex(1, quadrant, radius, 0);
	sphereDefinition->setVertex(2, radius, quadrant, 0);
	sphereDefinition->setVertex(3, radius, -quadrant, 0);
	sphereDefinition->setVertex(4, quadrant, -radius, 0);
	sphereDefinition->setVertex(5, -quadrant, -radius, 0);
	sphereDefinition->setVertex(6, -radius, -quadrant, 0);
	sphereDefinition->setVertex(7, -radius, quadrant, 0);

	// Create the line data.
	sphereDefinition->allocateLines(NUMBER_OF_POINTS);
	int startVertex = 0;
	int endVertex = 1;
	for (int i = 0; i < NUMBER_OF_POINTS; i++) {
		sphereDefinition->setLineVertexMapping(i, startVertex++, endVertex);
		if (endVertex < (NUMBER_OF_POINTS - 1)) {
			endVertex++;
		} else {
			// Close the circle.
			endVertex = 0;
		}
	}

	sphereDefinition->calculateBoundingSphere();
	this->setSpriteDefinition(sphereDefinition);
}

BoundingSphere::~BoundingSphere() {
	if (sphereDefinition) {
		delete sphereDefinition;
		sphereDefinition = NULL;
	}
}

void BoundingSphere::update(float elapsedTime) {
	this->moveTo(parent->getAnchorPointWCS(anchorIndex));
}
