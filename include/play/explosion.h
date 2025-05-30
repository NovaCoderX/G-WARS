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
#ifndef __EXPLOSION_H
#define __EXPLOSION_H

class Explosion: public MatrixObject {
	friend class ExplosionController;
	friend class GridLine;
public:
	Explosion(PlayState* playState, int numParticles, float maxVelocity, float duration, bool lightEmitting);
	~Explosion();

	float getBoundingSphere() const {
		return (boundingSphereExpansionFactor * totalElapsedTime);
	}

	const NovaVertex& getPositionCCS() const {
		return positionCCS;
	}

	virtual const NovaColor& getExplosionColor() const {
		return explosionColor;
	}

	void setExplosionColor(const NovaColor &color);

	bool isLightEmitting() const {
		return lightEmitting;
	}

	void setLightEmitting(bool lightEmitting) {
		this->lightEmitting = lightEmitting;
	}

	void calculateVisibility();
	void update(float elapsedTime);
	void draw();

private:
	PlayState* playState;
	Explosion *nextInList;
	Explosion *priorInList;
	NovaParticle *particles;
	int numParticles;
	float duration;
	float totalElapsedTime;
	NovaColor explosionColor;
	float colorDegredationFactor;
	float boundingSphereExpansionFactor;
	NovaVertex positionCCS;
	bool lightEmitting;
};

#endif // __EXPLOSION_H
