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
#ifndef __SPRITE_H
#define __SPRITE_H

enum Direction {
    UP = 0,
	UP_AND_RIGHT = -45,
	RIGHT = -90,
	DOWN_AND_RIGHT = -135,
	DOWN = 180,
    DOWN_AND_LEFT = 135,
	LEFT = 90,
	UP_AND_LEFT = 45
};

class Sprite: public MatrixObject {
public:
	Sprite(GameState* gameState);
	~Sprite();

	GameState* getGameState() {
		return gameState;
	}

	SpriteDefinition* getDefinition() {
		return definition;
	}

	void setSpriteDefinition(SpriteDefinition *definition);
	void setSpriteDefinition(const char *name);

	const NovaColor& getCurrentColor() const {
		return currentColor;
	}

	void setCurrentColor(const NovaColor &color) {
		this->currentColor = color;
	}

	float getBoundingSphere() const {
		return definition->getBoundingSphere();
	}

	const NovaVertex& getPositionCCS() const {
		return positionCCS;
	}

	void setFacingTowardsDirection(double direction);

	double getFacingTowardsDirection() const {
		return facingTowardsDirection;
	}

	double getHorizontalVelocity() const {
		return horizontalVelocity;
	}

	double getVerticalVelocity() const {
		return verticalVelocity;
	}

	double getTotalVelocity() const {
		return std::sqrt((horizontalVelocity * horizontalVelocity) + (verticalVelocity * verticalVelocity));
	}

	void reverseHorizontalVelocity() {
		horizontalVelocity = -horizontalVelocity;
	}

	void reverseVerticalVelocity() {
		verticalVelocity = -verticalVelocity;
	}

	void setVelocityFromComponents(double horizontalVelocity, double verticalVelocity) {
		this->horizontalVelocity = horizontalVelocity;
		this->verticalVelocity = verticalVelocity;
	}

	void setVelocityFromDirection(double direction, double totalVelocity);
	void applyVelocity(float elapsedTime);

	NovaVertex getAnchorPointWCS(int vertexIndex) const;
	NovaVertex getAnchorPointCCS(int vertexIndex) const;

	virtual void update(float elapsedTime) {}
	virtual void calculateVisibility();
	virtual void draw();

protected:
	GameState* gameState;

	// This same sprite definition can be shared by many sprites.
	SpriteDefinition *definition;

	// Vertex data (stored in CCS).
	RenderVertex *renderVertices;

	// Vertex data (stored in VCS).
	DisplayVertex* displayVertices;

	// Line data uses the (shared) vertex data.
	NovaLine *lines;

	NovaColor currentColor;
	NovaVertex positionCCS;
	double facingTowardsDirection;
	double horizontalVelocity;
	double verticalVelocity;
};

#endif // __SPRITE_H
