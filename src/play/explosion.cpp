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

enum ExplosionDuration {
	INNER_DURATION = 0, MIDDLE_DURATION = 1, OUTER_DURATION = 2
};

Explosion::Explosion(PlayState* playState, int numParticles, float maxVelocity, float duration) {
	this->playState = playState;
	nextInList = priorInList = NULL;

	if (numParticles <= 0) {
		fatalError("Invalid number of innerParticles\n");
	}

	if (maxVelocity <= 0) {
		fatalError("Invalid explosion maximum velocity\n");
	}

	if (duration <= 0) {
		fatalError("Invalid explosion duration\n");
	}

	this->numParticles = numParticles;
	innerParticles = new NovaParticle[this->numParticles];
	middleParticles = new NovaParticle[this->numParticles];
	outerParticles = new NovaParticle[this->numParticles];

	for (int i = 0; i < numParticles; i++) {
		innerParticles[i].horizontalVelocity = float_rand(-maxVelocity, maxVelocity);
		innerParticles[i].verticalVelocity = float_rand(-maxVelocity, maxVelocity);
		middleParticles[i].horizontalVelocity = float_rand(-maxVelocity, maxVelocity);
		middleParticles[i].verticalVelocity = float_rand(-maxVelocity, maxVelocity);
		outerParticles[i].horizontalVelocity = float_rand(-maxVelocity, maxVelocity);
		outerParticles[i].verticalVelocity = float_rand(-maxVelocity, maxVelocity);
	}

	this->duration[INNER_DURATION] = duration / 4;
	this->duration[MIDDLE_DURATION] = duration / 2;
	this->duration[OUTER_DURATION] = duration;
	totalFadeAmount = 0;

	/*
	 ** Iterate through the innerParticles
	 ** and store the distance of the vertex farthest away
	 ** from the explosion's origin.
	 */
	float farthest = 0;

	for (int i = 0; i < numParticles; i++) {
		NovaVertex particleLocation;

		particleLocation.x = (innerParticles[i].horizontalVelocity * duration);
		particleLocation.y = (innerParticles[i].verticalVelocity * duration);
		particleLocation.z = 0;

		if (particleLocation.magnitude() > farthest) {
			farthest = particleLocation.magnitude();
		}
	}

	// Calculate how much we need to grow the bounding sphere (each update) to contain the expanding innerParticles.
	boundingSphereExpansionFactor = (farthest / duration);

	lightEmitting = false;
	totalElapsedTime = 0;
}

Explosion::~Explosion() {
	if (innerParticles) {
		delete[] innerParticles;
		innerParticles = NULL;
	}

	if (middleParticles) {
		delete[] middleParticles;
		middleParticles = NULL;
	}

	if (outerParticles) {
		delete[] outerParticles;
		outerParticles = NULL;
	}
}

void Explosion::setColor(const NovaColor& color) {
	this->explosionColor = color;

	// Seed.
	float hightest = explosionColor.data[ColorIndex::RED];

	if (explosionColor.data[ColorIndex::GREEN] > hightest) {
		hightest = explosionColor.data[ColorIndex::GREEN];
	}

	if (explosionColor.data[ColorIndex::BLUE] > hightest) {
		hightest = explosionColor.data[ColorIndex::BLUE];
	}

	if (hightest <= 0) {
		fatalError("Invalid explosion starting color\n");
	}

	totalFadeAmount = hightest;

	// Reset.
	totalElapsedTime = 0;
}

void Explosion::update(float elapsedTime) {
	static NovaColor black;

	totalElapsedTime += elapsedTime;

	// Amount to reduce this tick.
	float fadeAmount = (totalFadeAmount / duration[OUTER_DURATION]) * elapsedTime;

	if (explosionColor.fade(fadeAmount, black)) {
		playState->getExplosionController()->deactivate(this);
	} else {
		// Mark this object visible/invisible for this frame.
		this->calculateVisibility();
	}

}

void Explosion::calculateVisibility() {
	positionCCS = playState->getCamera()->getPositionCCS(this);

	// If the object's origin is within the view port then it's definitely visible.
	if (checkProjectedPoint(positionCCS)) {
		visible = true;
	} else {
		// Take the objects extents into account.
		NovaVertex topR = NovaVertex(getBoundingSphere() + positionCCS.x, getBoundingSphere() + positionCCS.y, positionCCS.z);
		NovaVertex botL = NovaVertex(-getBoundingSphere() + positionCCS.x, -getBoundingSphere() + positionCCS.y, positionCCS.z);
		visible = checkProjectedPoints(topR, botL);
	}
}

void Explosion::draw() {
	static NovaVertex particle;
	static DisplayVertex displayVertex;

	glColor3fv(explosionColor.data);

	for (int i = 0; i < numParticles; i++) {
		particle = positionCCS;
		particle.x += (outerParticles[i].horizontalVelocity * totalElapsedTime);
		particle.y += (outerParticles[i].verticalVelocity * totalElapsedTime);

		// Transform.
		displayVertex.x = horizontalProject(particle.x, particle.z);
		displayVertex.y = verticalProject(particle.y, particle.z);

		glVertex2i(displayVertex.x, displayVertex.y);
	}

	if (totalElapsedTime < duration[MIDDLE_DURATION]) {
		for (int i = 0; i < numParticles; i++) {
			particle = positionCCS;
			particle.x += (middleParticles[i].horizontalVelocity * totalElapsedTime);
			particle.y += (middleParticles[i].verticalVelocity * totalElapsedTime);

			// Transform.
			displayVertex.x = horizontalProject(particle.x, particle.z);
			displayVertex.y = verticalProject(particle.y, particle.z);

			glVertex2i(displayVertex.x, displayVertex.y);
		}

		if (totalElapsedTime < duration[INNER_DURATION]) {
			for (int i = 0; i < numParticles; i++) {
				particle = positionCCS;
				particle.x += (innerParticles[i].horizontalVelocity * totalElapsedTime);
				particle.y += (innerParticles[i].verticalVelocity * totalElapsedTime);

				// Transform.
				displayVertex.x = horizontalProject(particle.x, particle.z);
				displayVertex.y = verticalProject(particle.y, particle.z);

				glVertex2i(displayVertex.x, displayVertex.y);
			}
		}
	}
}

