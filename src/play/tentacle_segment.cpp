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

TentacleSegment::TentacleSegment(PlayState* playState, Sprite* parent, float radius, const NovaColor &color, bool explosive)
: AlienComponent(playState, parent) {
	defaultColor = color;
	disabledColor = NovaColor(64, 64, 64);
	this->setSpriteColor(defaultColor);

	if (explosive) {
		this->setExplosionSize(SMALL_EXPLOSION);
		this->setExplosionColor(this->getSpriteColor());
	}

	// Create the definition.
	segmentDefinition = new SpriteDefinition();

	// Create the vertex data.
	float quadrant = (radius / 2);
	segmentDefinition->allocateVertices(NUMBER_OF_POINTS);
	segmentDefinition->setVertex(0, -quadrant, radius, 0);
	segmentDefinition->setVertex(1, quadrant, radius, 0);
	segmentDefinition->setVertex(2, radius, quadrant, 0);
	segmentDefinition->setVertex(3, radius, -quadrant, 0);
	segmentDefinition->setVertex(4, quadrant, -radius, 0);
	segmentDefinition->setVertex(5, -quadrant, -radius, 0);
	segmentDefinition->setVertex(6, -radius, -quadrant, 0);
	segmentDefinition->setVertex(7, -radius, quadrant, 0);

	// Create the line data.
	segmentDefinition->allocateLines(NUMBER_OF_POINTS);
	int startVertex = 0;
	int endVertex = 1;
	for (int i = 0; i < NUMBER_OF_POINTS; i++) {
		segmentDefinition->setLineVertexMapping(i, startVertex++, endVertex);
		if (endVertex < (NUMBER_OF_POINTS - 1)) {
			endVertex++;
		} else {
			// Close the circle.
			endVertex = 0;
		}
	}

	segmentDefinition->calculateBoundingSphere();
	this->setSpriteDefinition(segmentDefinition);
}

TentacleSegment::~TentacleSegment() {
	if (segmentDefinition) {
		delete segmentDefinition;
		segmentDefinition = NULL;
	}
}

void TentacleSegment::setDisabled(bool disabled) {
	// Base processing.
	AlienComponent::setDisabled(disabled);

	if (disabled) {
		this->setSpriteColor(disabledColor);
	} else {
		this->setSpriteColor(defaultColor);
	}
}
