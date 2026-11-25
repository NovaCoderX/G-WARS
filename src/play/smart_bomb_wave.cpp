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

#define TWO_PI 6.28318531f

// Number of particles in the ring.
#define DEFAULT_NUM_PARTICLES 256

// Ring speed in world units per tick (a tick is 1/10 s). The play area is 200 x 160,
// so the wave (wake included) clears the grid in about 1.3 s from the centre and
// 2.3 s from a corner. Aliens hold their fire for as long as it runs.
#define DEFAULT_WAVE_SPEED 9.0f

// The slowest particle travels at this fraction of the ring speed. The spread is what
// gives the ring its thickness, which grows as it expands.
#define SLOWEST_PARTICLE 0.92f

// Light profile around the ring, in world units: a short sharp rise ahead of the
// front and a longer glowing wake behind it. The grid spacing is 20, so the band is
// always wide enough to light at least one row of vertices.
#define LIGHT_LEAD 10.0f
#define LIGHT_TRAIL 45.0f

// How much the ring dims by the time it reaches the farthest corner.
#define END_BRIGHTNESS 0.35f

// Light added to the grid relative to the particle color.
#define LIGHT_SCALE 1.0f

SmartBombWave::SmartBombWave(PlayState* playState) {
	this->playState = playState;
	directionX = NULL;
	directionY = NULL;
	numParticles = 0;
	speed = DEFAULT_WAVE_SPEED;
	radius = 0;
	maxRadius = 0;
	brightness = 0;
	active = false;
	innerRadiusSquared = 0;
	outerRadiusSquared = 0;
}

SmartBombWave::~SmartBombWave() {
	if (directionX) {
		delete[] directionX;
		directionX = NULL;
	}

	if (directionY) {
		delete[] directionY;
		directionY = NULL;
	}
}

void SmartBombWave::init() {
	Yamlish* yamlish = g_worldManager->getYamlish();

	numParticles = yamlish->getInt("smartbomb.particle.density", DEFAULT_NUM_PARTICLES);
	if (numParticles < 8) {
		numParticles = 8;
	}

	speed = yamlish->getFloat("smartbomb.particle.velocity", DEFAULT_WAVE_SPEED);
	if (speed <= 0) {
		speed = DEFAULT_WAVE_SPEED;
	}

	// A pale electric blue.
	baseColor = NovaColor(140, 190, 255);

	directionX = new float[numParticles];
	directionY = new float[numParticles];

	// Evenly spaced round the circle with a little angular jitter so it does not look
	// like a regular polygon, each with its own speed. All the trigonometry is done
	// here, once; per frame a particle costs two multiplies and two adds.
	float step = TWO_PI / numParticles;
	for (int i = 0; i < numParticles; i++) {
		float angle = (i * step) + float_rand(-step / 2, step / 2);
		float factor = float_rand(SLOWEST_PARTICLE, 1.0f);

		directionX[i] = cosf(angle) * factor;
		directionY[i] = sinf(angle) * factor;
	}
}

void SmartBombWave::trigger(const NovaVertex &originWCS) {
	this->originWCS = originWCS;
	this->originWCS.z = 0;

	// The wave is finished when its wake has passed the play area corner farthest
	// from the origin.
	float dx = (fabsf(originWCS.x) + VERTICAL_BORDER_POSITION);
	float dy = (fabsf(originWCS.y) + HORIZONTAL_BORDER_POSITION);
	maxRadius = sqrtf((dx * dx) + (dy * dy)) + LIGHT_TRAIL;

	radius = 0;
	brightness = 1;
	active = true;

	// Make sure draw() has a valid CCS origin even if it runs before update().
	this->update(0);
}

void SmartBombWave::update(float elapsedTime) {
	if (!active) {
		return;
	}

	radius += (speed * elapsedTime);
	if (radius >= maxRadius) {
		active = false;
		return;
	}

	// The camera follows the player, so the origin has to be re-projected every frame.
	originCCS = (originWCS * playState->getCamera()->getWorldToCameraMatrix());

	brightness = 1.0f - ((1.0f - END_BRIGHTNESS) * (radius / maxRadius));

	particleColor = baseColor;
	particleColor *= brightness;

	lightColor = baseColor;
	lightColor *= (brightness * LIGHT_SCALE);

	float inner = radius - LIGHT_TRAIL;
	if (inner < 0) {
		inner = 0;
	}

	float outer = radius + LIGHT_LEAD;

	innerRadiusSquared = (inner * inner);
	outerRadiusSquared = (outer * outer);
}

/*
 ** Called inside the GL_POINTS pass.
 */
void SmartBombWave::draw() {
	if (!active) {
		return;
	}

	glColor3fv(particleColor.data);

	for (int i = 0; i < numParticles; i++) {
		float offsetX = (directionX[i] * radius);
		float offsetY = (directionY[i] * radius);

		// Keep the ring inside the play area: it reads as a wave passing over the
		// grid, and it fades out piece by piece as it reaches the borders.
		float wcsX = originWCS.x + offsetX;
		float wcsY = originWCS.y + offsetY;
		if ((wcsX <= -VERTICAL_BORDER_POSITION) || (wcsX >= VERTICAL_BORDER_POSITION) ||
			(wcsY <= -HORIZONTAL_BORDER_POSITION) || (wcsY >= HORIZONTAL_BORDER_POSITION)) {
			continue;
		}

		// No rotation in the camera, so an offset in WCS is the same offset in CCS.
		glVertex2i(horizontalProject(originCCS.x + offsetX, originCCS.z),
				verticalProject(originCCS.y + offsetY, originCCS.z));
	}
}

/*
 ** Add the wave's light to a vertex (in CCS) on the play area plane.
 */
void SmartBombWave::applyLighting(RenderVertex &targetVertex) const {
	if (!active) {
		return;
	}

	float dx = (targetVertex.x - originCCS.x);
	float dy = (targetVertex.y - originCCS.y);
	float distanceSquared = (dx * dx) + (dy * dy);

	// Most vertices are nowhere near the ring: reject them without a square root.
	if ((distanceSquared <= innerRadiusSquared) || (distanceSquared >= outerRadiusSquared)) {
		return;
	}

	float delta = sqrtf(distanceSquared) - radius;
	float amount;
	if (delta >= 0) {
		amount = 1.0f - (delta / LIGHT_LEAD);
	} else {
		amount = 1.0f + (delta / LIGHT_TRAIL);
	}

	if (amount <= 0) {
		return;
	}

	NovaColor light = lightColor;
	light *= amount;
	targetVertex.lightSum += light;
}
