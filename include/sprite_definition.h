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

#ifndef __SPRITE_DEFINITION_H
#define __SPRITE_DEFINITION_H

const int MAX_DEF_NAME = 40;

struct LineVertexMap {
	int vertexIndex[2];
};

class SpriteDefinition {
public:
	SpriteDefinition(const char *name = 0);
	~SpriteDefinition();

	const char* getName() const {
		return name;
	}

	void setName(const char *defName) {
		strcpy(name, defName);
	}

	int getNumVertices() const {
		return numVertices;
	}

	int getNumLines() const {
		return numLines;
	}

	void allocateVertices(int vertices);
	void setVertex(int eleNum, float x, float y, float z);

	void allocateLines(int lines);
	void setLineMapping(int lineIndex, int startVertex, int endVertex);

	void scaleVertices(float scaleFactor);

	void calculateBoundingSphere();

	float getBoundingSphere() const {
		return boundingSphere;
	}

	/// Shared vertex data (stored in LCS).
	NovaVertex *staticVertices;

	// Line to vertex mapping data.
	LineVertexMap *lineVertexMap;

private:
	char name[MAX_DEF_NAME];
	int numVertices;
	int numLines;
	float boundingSphere;
};

#endif // __SPRITE_DEFINITION_H
