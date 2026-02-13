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

Camera::Camera() {
	worldToCamera.MakeIdentity();

	// Note, hard-coded camera starting position.
	this->moveTo(NovaVertex(0, 0, -120));

	this->calculateWorldToCameraMatrix();
}

// Calculates the inverse of the camera LCS->WCS matrix.
// This is stored as the camera's worldToObject matrix which
// can be multiplied by an objects LCS->WCS matrix to compute THAT objects
// LCS->CCS matrix.
void Camera::calculateWorldToCameraMatrix() {
	NovaVertex position;
	AstMatrix3x3 rm3x3;

	/*
	 ** Get camera's rotation matrix and translation.
	 */
	this->getRotationMatrixWCS(rm3x3);

	/*
	 ** Invert the rotation matrix (a transpose is the
	 ** the same as an inversion when working with
	 ** rotation matrices).
	 */
	rm3x3.Transpose();

	/*
	 ** Transform its location by the inverted rotation matrix
	 */
	position = this->getPositionWCS() * rm3x3;

	/*
	 ** Create world_to_object_matrix
	 */
	worldToCamera.SetRotationMatrix(rm3x3);
	worldToCamera.SetTranslation(-position);
	worldToCamera.NormalizeDirectionVectors();
}

