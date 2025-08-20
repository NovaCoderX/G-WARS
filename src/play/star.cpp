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

Star::Star(PlayState* playState) {
	this->playState = playState;
}

void Star::init(const NovaColor &color, float depth, float horizontalExtents, float verticalExtents) {
	highColor = color;

	// Randomize the starting color.
	lowColor = highColor;
	lowColor.rebase(float_rand(10, 50));
	starColor = lowColor;

	this->moveTo(int_rand(-horizontalExtents, horizontalExtents), int_rand(-verticalExtents, verticalExtents), depth);
}

void Star::update(float elapsedTime) {
	static bool increasingColor = true;
	static NovaColor black;

	// Need to transform this objects location into a CCS location.
	// This will then be our center point for the star.
	// We can use this center point to create 4 edges of the star and then project and draw all 5 points.
	positionCCS = playState->getCamera()->getPositionCCS(this);

	visible = playState->getCamera()->checkProjectedPoint(positionCCS);
	if (visible) {
		// Twinkle - color cycle
		if (increasingColor) {
			if (starColor.brighten((elapsedTime / 10), highColor)) {
				increasingColor = false;
			}
		} else {
			if (starColor.fade((elapsedTime / 10), black)) {
				increasingColor = true;
			}
		}
	}
}

void Star::draw() {
	static DisplayVertex displayVertex;

	// Draw the center pixel.
	glColor3fv(starColor.data);
	displayVertex.x = horizontalProject(positionCCS.x, positionCCS.z);
	displayVertex.y = verticalProject(positionCCS.y, positionCCS.z);
	glVertex2i(displayVertex.x, displayVertex.y);

	// Then draw the outer pixels (if the star is far enough away).
	if (positionCCS.z > 300) {
		glColor3fv(lowColor.data);
		displayVertex.x = horizontalProject(positionCCS.x + 1, positionCCS.z);
		displayVertex.y = verticalProject(positionCCS.y, positionCCS.z);
		glVertex2i(displayVertex.x, displayVertex.y);

		displayVertex.x = horizontalProject(positionCCS.x, positionCCS.z);
		displayVertex.y = verticalProject(positionCCS.y - 1, positionCCS.z);
		glVertex2i(displayVertex.x, displayVertex.y);

		displayVertex.x = horizontalProject(positionCCS.x - 1, positionCCS.z);
		displayVertex.y = verticalProject(positionCCS.y, positionCCS.z);
		glVertex2i(displayVertex.x, displayVertex.y);

		displayVertex.x = horizontalProject(positionCCS.x, positionCCS.z);
		displayVertex.y = verticalProject(positionCCS.y + 1, positionCCS.z);
		glVertex2i(displayVertex.x, displayVertex.y);
	}
}

