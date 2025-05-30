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

SpriteDefinition::SpriteDefinition(const char *name) {
	if (name) {
		setName(name);
	} else {
		setName("UNDEFINED");
	}

	staticVertices = NULL;
	numVertices = 0;
	lineVertexMap = NULL;
	numLines = 0;
	boundingSphere = 0;
}

SpriteDefinition::~SpriteDefinition() {
	if (staticVertices) {
		delete[] staticVertices;
		staticVertices = NULL;
	}

	if (lineVertexMap) {
		delete[] lineVertexMap;
		lineVertexMap = NULL;
	}
}

void SpriteDefinition::allocateVertices(int vertices) {
	numVertices = vertices;

	staticVertices = new NovaVertex[numVertices];
	if (!staticVertices) {
		fatalError("Could not allocate the static vertex array");
	}
}

void SpriteDefinition::setVertex(int index, float x, float y, float z) {
	if (index < 0 || index >= numVertices) {
		fatalError("Vertex index out of range\n");
	}

	staticVertices[index].x = x;
	staticVertices[index].y = y;
	staticVertices[index].z = z;
}

void SpriteDefinition::allocateLines(int lines) {
	numLines = lines;

	lineVertexMap = new LineVertexMap[numLines];
	if (!lineVertexMap) {
		fatalError("Could not allocate line_array");
	}
}

void SpriteDefinition::setLineMapping(int index, int startVertex, int endVertex) {
	if (index < 0 || index >= numLines) {
		fatalError("Line index out of range\n");
	}

	if (startVertex < 0 || startVertex >= numVertices) {
		fatalError("Vertex index out of range\n");
	}

	if (endVertex < 0 || endVertex >= numVertices) {
		fatalError("Vertex index out of range\n");
	}

	// We need to record that this line index uses these two vertex indexes.
	lineVertexMap[index].vertexIndex[BEGIN] = startVertex;
	lineVertexMap[index].vertexIndex[END] = endVertex;
}

void SpriteDefinition::scaleVertices(float scaleFactor) {
	if (scaleFactor != 0) {
		if (scaleFactor < 0) {
			for (int i = 0; i < numVertices; i++) {
				staticVertices[i].x = (staticVertices[i].x / fabs(scaleFactor));
				staticVertices[i].y = (staticVertices[i].y / fabs(scaleFactor));
				staticVertices[i].z = (staticVertices[i].z / fabs(scaleFactor));
			}
		} else {
			for (int i = 0; i < numVertices; i++) {
				staticVertices[i].x = (staticVertices[i].x * scaleFactor);
				staticVertices[i].y = (staticVertices[i].y * scaleFactor);
				staticVertices[i].z = (staticVertices[i].z * scaleFactor);
			}
		}
	}
}

void SpriteDefinition::calculateBoundingSphere() {
	// Sanity check.
	if (!staticVertices) {
		fatalError("Cannot calculate a bounding sphere without any vertices");
	}

	/*
	 ** Iterate through the sprite definition's vertices,
	 ** and store the distance of the vertex farthest away
	 ** from the object's origin.
	 */
	float farthest = 0;

	for (int i = 0; i < this->getNumVertices(); i++) {
		if (staticVertices[i].magnitude() > farthest) {
			farthest = staticVertices[i].magnitude();
		}
	}

	boundingSphere = farthest;
}

