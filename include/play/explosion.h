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


enum ExplosionSize {
	NO_EXPLOSION = 0,
	SMALL_EXPLOSION = 1,
	MEDIUM_EXPLOSION = 2,
	LARGE_EXPLOSION = 3,
	MASSIVE_EXPLOSION = 4
};

class ExplosiveObject {
public:
	ExplosiveObject() {
		size = NO_EXPLOSION;
		sound = SILENCE;
		lightEmitting = false;
	}

	virtual ~ExplosiveObject() {}

	ExplosionSize getExplosionSize() const {
		return size;
	}

	void setExplosionSize(ExplosionSize size) {
		this->size = size;
	}

	SampleType getExplosionSound() const {
		return sound;
	}

	void setExplosionSound(SampleType sound) {
		this->sound = sound;
	}

	bool isLightEmittingExplosion() const {
		return lightEmitting;
	}

	void setLightEmittingExplosion(bool lightEmitting) {
		this->lightEmitting = lightEmitting;
	}

	virtual NovaVertex getExplosionOrigin() const = 0;
	virtual const NovaColor& getExplosionColor() const = 0;

private:
	ExplosionSize size;
	SampleType sound;
	bool lightEmitting;
};

class Explosion: public MatrixObject {
public:
	Explosion(PlayState* playState, int numParticles, float maxVelocity, float duration);
	~Explosion();

	float getBoundingSphere() const {
		return (boundingSphereExpansionFactor * totalElapsedTime);
	}

	const NovaVertex& getPositionCCS() const {
		return positionCCS;
	}

	virtual const NovaColor& getColor() const {
		return explosionColor;
	}

	void setColor(const NovaColor &color);

	bool isLightEmitting() const {
		return lightEmitting;
	}

	void setLightEmitting(bool lightEmitting) {
		this->lightEmitting = lightEmitting;
	}

	void update(float elapsedTime);
	void calculateVisibility();
	void draw();

public:
	Explosion *nextInList;
	Explosion *priorInList;

private:
	PlayState* playState;
	NovaParticle *innerParticles;
	NovaParticle *middleParticles;
	NovaParticle *outerParticles;
	int numParticles;
	float duration[3];
	float totalElapsedTime;
	NovaColor explosionColor;
	float totalFadeAmount;
	float boundingSphereExpansionFactor;
	NovaVertex positionCCS;
	bool lightEmitting;
};

#endif // __EXPLOSION_H
