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

MatrixObject::MatrixObject() {
	active = false;
	visible = false;
	objectToWorldMatrix.MakeIdentity();
}

MatrixObject::~MatrixObject() {
	// Empty.
}

/*
 ** the following routines handle translation by creating a
 ** 4x3 translation matrix and applying it to the objectToWorldMatrix.
 */
static AstMatrix4x3 translation_matrix;

void MatrixObject::moveAlongX(float amount) {
	translation_matrix.MakeIdentity();
	translation_matrix.MakeXTranslation(amount);
	objectToWorldMatrix = translation_matrix * objectToWorldMatrix;
}

void MatrixObject::moveAlongY(float amount) {
	translation_matrix.MakeIdentity();
	translation_matrix.MakeYTranslation(amount);
	objectToWorldMatrix = translation_matrix * objectToWorldMatrix;
}

void MatrixObject::moveAlongZ(float amount) {
	translation_matrix.MakeIdentity();
	translation_matrix.MakeZTranslation(amount);
	objectToWorldMatrix = translation_matrix * objectToWorldMatrix;
}

/*
 ** The following functions handle rotations.  Unlike translation,
 ** I have opted not to use straight matrix multiplies to implement
 ** rotation.  The reason for this is one of accuracy -- because of
 ** the limited accuracy of fixed point math (or floating point
 ** math, for that matter) matrix values tend to start skewing
 ** after many multiplies.  This may take a while to build up, but
 ** when it does the effects can be quite dramatic -- heavily
 ** distorted objects on screen.
 **
 ** To get around this we "fix" our matrices using an orthogonalization
 ** method whereby we compute the individual right, up, and forward
 ** vectors of the rotation matrix through cross products.  If this
 ** seems complicated -- don't worry about it, it works.
 */
static AstMatrix4x3 rotation_matrix;

void MatrixObject::rotateAroundX(float degrees) {
	float m10 = objectToWorldMatrix[1][0], m20 = objectToWorldMatrix[2][0], m11 = objectToWorldMatrix[1][1],
			m21 = objectToWorldMatrix[2][1];

	rotation_matrix.MakeXRotation(degrees);

	float sm11 = rotation_matrix[1][1], sm12 = rotation_matrix[1][2], sm21 = rotation_matrix[2][1], sm22 = rotation_matrix[2][2];
	/*
	 ** Compute column 0
	 */
	objectToWorldMatrix[1][0] = (sm11 * m10) + (sm12 * m20);
	objectToWorldMatrix[2][0] = (sm21 * m10) + (sm22 * m20);

	/*
	 ** Compute column 1
	 */
	objectToWorldMatrix[1][1] = (sm11 * m11) + (sm12 * m21);
	objectToWorldMatrix[2][1] = (sm21 * m11) + (sm22 * m21);

	/*
	 ** Compute column 2 (as col0 x col1 )
	 */
	objectToWorldMatrix[0][2] = (objectToWorldMatrix[1][0] * objectToWorldMatrix[2][1])
			- (objectToWorldMatrix[2][0] * objectToWorldMatrix[1][1]);
	objectToWorldMatrix[1][2] = (objectToWorldMatrix[2][0] * objectToWorldMatrix[0][1])
			- (objectToWorldMatrix[0][0] * objectToWorldMatrix[2][1]);
	objectToWorldMatrix[2][2] = (objectToWorldMatrix[0][0] * objectToWorldMatrix[1][1])
			- (objectToWorldMatrix[0][1] * objectToWorldMatrix[1][0]);

	/*
	 ** Recompute column 0 as col1 x col2
	 */
	NovaVertex vu, vf, new_vr;

	objectToWorldMatrix.GetVF(vf);
	objectToWorldMatrix.GetVU(vu);
	new_vr = vu.cross(vf);
	objectToWorldMatrix.SetVR(new_vr);
	objectToWorldMatrix.NormalizeDirectionVectors();
}

void MatrixObject::rotateAroundY(float degrees) {
	float m00 = objectToWorldMatrix[0][0], m20 = objectToWorldMatrix[2][0], m01 = objectToWorldMatrix[0][1],
			m21 = objectToWorldMatrix[2][1];

	rotation_matrix.MakeYRotation(degrees);

	float sm00 = rotation_matrix[0][0], sm02 = rotation_matrix[0][2], sm20 = rotation_matrix[2][0], sm22 = rotation_matrix[2][2];

	/*
	 ** Compute column 0
	 */
	objectToWorldMatrix[0][0] = (sm00 * m00) + (sm02 * m20);
	objectToWorldMatrix[2][0] = (sm20 * m00) + (sm22 * m20);

	/*
	 ** Compute column 1
	 */
	objectToWorldMatrix[0][1] = (sm00 * m01) + (sm02 * m21);
	objectToWorldMatrix[2][1] = (sm20 * m01) + (sm22 * m21);

	/*
	 ** Compute column 2 (as col0 x col1 )
	 */
	objectToWorldMatrix[0][2] = (objectToWorldMatrix[1][0] * objectToWorldMatrix[2][1])
			- (objectToWorldMatrix[2][0] * objectToWorldMatrix[1][1]);
	objectToWorldMatrix[1][2] = (objectToWorldMatrix[2][0] * objectToWorldMatrix[0][1])
			- (objectToWorldMatrix[0][0] * objectToWorldMatrix[2][1]);
	objectToWorldMatrix[2][2] = (objectToWorldMatrix[0][0] * objectToWorldMatrix[1][1])
			- (objectToWorldMatrix[0][1] * objectToWorldMatrix[1][0]);

	/*
	 ** Recompute column 0 (as col1 x col2 )
	 */
	NovaVertex vu, vf, new_vr;

	objectToWorldMatrix.GetVF(vf);
	objectToWorldMatrix.GetVU(vu);
	new_vr = vu.cross(vf);
	objectToWorldMatrix.SetVR(new_vr);
	objectToWorldMatrix.NormalizeDirectionVectors();
}

void MatrixObject::rotateAroundZ(float degrees) {
	float m00 = objectToWorldMatrix[0][0], m01 = objectToWorldMatrix[0][1], m10 = objectToWorldMatrix[1][0],
			m11 = objectToWorldMatrix[1][1];

	rotation_matrix.MakeZRotation(degrees);

	float sm00 = rotation_matrix[0][0], sm01 = rotation_matrix[0][1], sm10 = rotation_matrix[1][0], sm11 = rotation_matrix[1][1];

	/*
	 ** Compute column 0
	 */
	objectToWorldMatrix[0][0] = (sm00 * m00) + (sm01 * m10);

	objectToWorldMatrix[1][0] = (sm10 * m00) + (sm11 * m10);

	/*
	 ** Compute column 1
	 */
	objectToWorldMatrix[0][1] = (sm00 * m01) + (sm01 * m11);
	objectToWorldMatrix[1][1] = (sm10 * m01) + (sm11 * m11);

	/*
	 ** Compute column 2 (as col0 x col1 )
	 */
	objectToWorldMatrix[0][2] = (objectToWorldMatrix[1][0] * objectToWorldMatrix[2][1])
			- (objectToWorldMatrix[2][0] * objectToWorldMatrix[1][1]);
	objectToWorldMatrix[1][2] = (objectToWorldMatrix[2][0] * objectToWorldMatrix[0][1])
			- (objectToWorldMatrix[0][0] * objectToWorldMatrix[2][1]);
	objectToWorldMatrix[2][2] = (objectToWorldMatrix[0][0] * objectToWorldMatrix[1][1])
			- (objectToWorldMatrix[0][1] * objectToWorldMatrix[1][0]);

	/*
	 ** Recompute column 0 (as col1 x col2 )
	 */
	NovaVertex vu, vf, new_vr;

	objectToWorldMatrix.GetVF(vf);
	objectToWorldMatrix.GetVU(vu);
	new_vr = vu.cross(vf);
	objectToWorldMatrix.SetVR(new_vr);
	objectToWorldMatrix.NormalizeDirectionVectors();
}






