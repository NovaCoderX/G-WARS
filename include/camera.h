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
#ifndef __CAMERA_H
#define __CAMERA_H

// The projection values are hard-coded to our fixed screen resolution for performance.
#define CAMERA_HDF 554.256296
#define CAMERA_VDF 415.692234

class Camera: public MatrixObject {
public:
	Camera();

	void calculateWorldToCameraMatrix();

	AstMatrix4x3& getWorldToCameraMatrix() {
		return worldToCamera;
	}

	NovaVertex getPositionCCS(MatrixObject* object) const {
		return object->getPositionWCS() * worldToCamera;
	}

private:
	AstMatrix4x3 worldToCamera; // LCS->WCS (inverse).
};

// Inlined for performance.
static inline GLint horizontalProject(float value, float z) {
	return (GLint)((value * CAMERA_HDF) / z) + (DISPLAY_WITDH / 2);
}

static inline GLint verticalProject(float value, float z) {
	return (GLint)((value * CAMERA_VDF) / z) + (DISPLAY_HEIGHT / 2);
}

static inline bool checkProjectedPoint(const NovaVertex &location) {
	DisplayVertex dest;

	dest.x = horizontalProject(location.x, location.z);
	dest.y = verticalProject(location.y, location.z);

	if ((dest.x < 0 || dest.x > (DISPLAY_WITDH - 1)) || (dest.y < 0 || dest.y > (DISPLAY_HEIGHT - 1))) {
		return false;
	}

	return true;
}

static inline bool checkProjectedPoints(const NovaVertex &topRight, const NovaVertex &botLeft) {
	DisplayVertex topR, botL;

	topR.x = horizontalProject(topRight.x, topRight.z);
	topR.y = verticalProject(topRight.y, topRight.z);

	botL.x = horizontalProject(botLeft.x, botLeft.z);
	botL.y = verticalProject(botLeft.y, botLeft.z);

	if ((topR.x >= 0) && (botL.y < (DISPLAY_HEIGHT - 1)) && (botL.x < (DISPLAY_WITDH - 1)) && (topR.y >= 0)) {
		return true;
	}

	return false;
}

static inline bool checkProjectedPoints(const DisplayVertex &topRight, const  DisplayVertex &botLeft) {
	if ((topRight.x >= 0) && (botLeft.y < (DISPLAY_HEIGHT - 1)) && (botLeft.x < (DISPLAY_WITDH - 1)) && (topRight.y >= 0)) {
		return true;
	}

	return false;
}


#endif // __CAMERA_H
