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

Laser::Laser(PlayState* playState) {
	this->playState = playState;

	// Set up how this object will explode.
	this->setExplosionSize(SMALL_EXPLOSION);
	this->setExplosionSound(LASER_EXPLODE);
	this->setLightEmittingExplosion(false);
}

void Laser::draw() {
	static AstMatrix4x3 objectToCameraMatrix;

	// Update our composite LCS to CCS matrix that consists of the product of the object to world matrix and the
	// global world to camera matrix.
	objectToCameraMatrix = objectToWorldMatrix * playState->getCamera()->getWorldToCameraMatrix();

	renderVertices[LINE_BEGIN] = staticVertices[LINE_BEGIN] * objectToCameraMatrix;

	displayVertices[LINE_BEGIN].x = horizontalProject(renderVertices[LINE_BEGIN].x,
			renderVertices[LINE_BEGIN].z);
	displayVertices[LINE_BEGIN].y = verticalProject(renderVertices[LINE_BEGIN].y,
			renderVertices[LINE_BEGIN].z);

	renderVertices[LINE_END] = staticVertices[LINE_END] * objectToCameraMatrix;

	displayVertices[LINE_END].x = horizontalProject(renderVertices[LINE_END].x,
			renderVertices[LINE_END].z);
	displayVertices[LINE_END].y = verticalProject(renderVertices[LINE_END].y,
			renderVertices[LINE_END].z);

	glColor3fv(laserColor[LINE_BEGIN].data);
	glVertex2i(displayVertices[LINE_BEGIN].x, displayVertices[LINE_BEGIN].y);

	glColor3fv(laserColor[LINE_END].data);
	glVertex2i(displayVertices[LINE_END].x, displayVertices[LINE_END].y);
}

