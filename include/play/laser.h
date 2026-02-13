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
#ifndef __LASER_H
#define __LASER_H

class Laser: public MatrixObject, public ExplosiveObject {
public:
	Laser(PlayState* playState);

	void setVertex(LineIndex index, float x, float y, float z) {
		staticVertices[index].x = x;
		staticVertices[index].y = y;
		staticVertices[index].z = z;
	}

	void setColor(const NovaColor& color) {
		laserColor[LINE_BEGIN] = color;
		laserColor[LINE_END] = color;
		laserColor[LINE_END].rebase(50);
	}

	void setColor(LineIndex index, const NovaColor& color) {
		laserColor[index] = color;
	}

	// Overridden.
	NovaVertex getExplosionOrigin() const {
		return staticVertices[LINE_END];
	}

	// Overridden.
	const NovaColor& getExplosionColor() const {
		return laserColor[LINE_END];
	}

	void draw();

protected:
	PlayState* playState;

	// Vertex data (stored in LCS).
	NovaVertex staticVertices[2];

	// Vertex data (stored in CCS).
	RenderVertex renderVertices[2];

	// Vertex data (stored in VCS).
	DisplayVertex displayVertices[2];

	NovaColor laserColor[2];
};

#endif // __LASER_H
