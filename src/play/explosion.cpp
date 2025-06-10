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

Explosion::Explosion(PlayState* playState, int numParticles, float maxVelocity, float duration, bool lightEmitting) {
	this->playState = playState;
	nextInList = priorInList = NULL;
	
	if (numParticles <= 0) {
		fatalError("Invalid number of particles\n");
	}

	if (maxVelocity <= 0) {
		fatalError("Invalid explosion maximum velocity\n");
	}

	if (duration <= 0) {
		fatalError("Invalid explosion duration\n");
	}

	this->numParticles = numParticles;
	particles = new NovaParticle[this->numParticles];

	for (int i = 0; i < numParticles; i++) {
		particles[i].horizontalVelocity = float_rand(-maxVelocity, maxVelocity);
		particles[i].verticalVelocity = float_rand(-maxVelocity, maxVelocity);
	}

	this->duration = duration;
	colorDegredationFactor = 0;

	/*
	 ** Iterate through the particles
	 ** and store the distance of the vertex farthest away
	 ** from the explosion's origin.
	 */
	float farthest = 0;

	for (int i = 0; i < numParticles; i++) {
		NovaVertex particleLocation;

		particleLocation.x = (particles[i].horizontalVelocity * duration);
		particleLocation.y = (particles[i].verticalVelocity * duration);
		particleLocation.z = 0;

		if (particleLocation.magnitude() > farthest) {
			farthest = particleLocation.magnitude();
		}
	}

	// Calculate how much we need to grow the bounding sphere (each update) to contain the expanding particles.
	boundingSphereExpansionFactor = (farthest / duration);

	this->lightEmitting = lightEmitting;

	totalElapsedTime = 0;
}

Explosion::~Explosion() {
	if (particles) {
		delete[] particles;
		particles = NULL;
	}
}

void Explosion::setExplosionColor(const NovaColor &color) {
	this->explosionColor = color;

	// Seed.
	float highest = explosionColor.data[ColorIndex::RED];

	if (explosionColor.data[ColorIndex::GREEN] > highest) {
		highest = explosionColor.data[ColorIndex::GREEN];
	}

	if (explosionColor.data[ColorIndex::BLUE] > highest) {
		highest = explosionColor.data[ColorIndex::BLUE];
	}

	if (highest <= 0) {
		fatalError("Invalid explosion starting color\n");
	}

	// Calculate how much color we need to degrade (each update) to get to zero at 'max elapsed time'.
	colorDegredationFactor = (highest / duration);

	// Reset.
	totalElapsedTime = 0;
}

void Explosion::calculateVisibility() {
	positionCCS = playState->getCamera()->getPositionCCS(this);

	// If the object's origin is within the view port then it's definitely visible.
	if (playState->getCamera()->checkProjectedPoint(positionCCS)) {
		visible = true;
	} else {
		// Take the objects extents into account.
		NovaVertex topR = NovaVertex(getBoundingSphere() + positionCCS.x, getBoundingSphere() + positionCCS.y, positionCCS.z);
		NovaVertex botL = NovaVertex(-getBoundingSphere() + positionCCS.x, -getBoundingSphere() + positionCCS.y,
				positionCCS.z);
		visible = playState->getCamera()->checkProjectedPoints(topR, botL);
	}
}

void Explosion::update(float elapsedTime) {
	static NovaColor black;

	// Based on the elapsed time, update the position of everything.
	totalElapsedTime += elapsedTime;

	if (totalElapsedTime >= duration) {
		// All of the particles should have now faded to black.
		playState->getExplosionController()->deactivate(this);

		// Reset.
		totalElapsedTime = 0;
	} else {
		// Mark this object visible/invisible for this frame.
		this->calculateVisibility();

		// Degrade the explosionColor (even if he explosion isn't currently visible to stay in sync).
		explosionColor.fade(colorDegredationFactor * elapsedTime, black);
	}
}

void Explosion::draw() {
	static NovaVertex particle;
	static DisplayVertex displayVertex;

	glColor3fv(explosionColor.data);

	for (int i = 0; i < numParticles; i++) {
		particle = positionCCS;
		particle.x += (particles[i].horizontalVelocity * totalElapsedTime);
		particle.y += (particles[i].verticalVelocity * totalElapsedTime);

		// Transform.
		displayVertex.x = horizontalProject(particle.x, particle.z);
	   	displayVertex.y = verticalProject(particle.y, particle.z);

		glVertex2i(displayVertex.x, displayVertex.y);
	}
}

