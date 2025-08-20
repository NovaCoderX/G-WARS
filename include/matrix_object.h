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
#ifndef __MATRIX_OBJECT_H
#define __MATRIX_OBJECT_H

class MatrixObject {
public:
	MatrixObject();
	virtual ~MatrixObject();

	bool isActive() const {
		return active;
	}

	virtual void setActive(bool active) {
		this->active = active;

		if (active) {
			// Prevent object being drawn when the draw method is called before the update method.
			visible = false;
		}
	}

	bool isVisible() const {
		return visible;
	}

	void setVisible(bool visible) {
		this->visible = visible;
	}

	NovaVertex getPositionWCS() const {
		return objectToWorldMatrix.GetTranslation();
	}

	void getRotationMatrixWCS(AstMatrix3x3 &rm3x3) const {
		objectToWorldMatrix.GetRotationMatrix(rm3x3);
	}

	void setRotationMatrix(const AstMatrix3x3 &rm3x3) {
		objectToWorldMatrix.SetRotationMatrix(rm3x3);
	}

	void moveAlongX(float amount);
	void moveAlongY(float amount);
	void moveAlongZ(float amount);

	void moveTo(const NovaVertex &newposition) {
		objectToWorldMatrix.SetTranslation(newposition);
	}

	void moveTo(float x, float y, float z) {
		moveTo(NovaVertex(x, y, z));
	}

	void rotateAroundX(float degrees);
	void rotateAroundY(float degrees);
	void rotateAroundZ(float degrees);

protected:
	bool active;
	bool visible;

	// LCS->WCS matrix.
	AstMatrix4x3 objectToWorldMatrix;
};

#endif // __MATRIX_OBJECT_H
