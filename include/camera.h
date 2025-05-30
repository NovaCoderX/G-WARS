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

	bool checkProjectedPoint(const NovaVertex& point) const;
	bool checkProjectedPoints(const NovaVertex& topRight, const NovaVertex& botLeft) const;
	bool checkProjectedPoints(const DisplayVertex& topRight, const DisplayVertex& botLeft) const;

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

#endif // __CAMERA_H
