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
#ifndef __SMART_BOMB_WAVE_H
#define __SMART_BOMB_WAVE_H

/*
 ** The smart bomb shockwave: a ring of particles that starts at the player and
 ** expands until it has swept the whole play area, lighting the grid vertices
 ** it passes over.
 **
 ** Everything is done in the plane of the play area (z = 0). The camera never
 ** rotates, so distances measured in CCS are the same as in WCS and the light
 ** test can work directly on the grid's transformed vertices.
 */
class SmartBombWave {
public:
	SmartBombWave(PlayState* playState);
	~SmartBombWave();

	bool isActive() const {
		return active;
	}

	void init();
	void trigger(const NovaVertex &originWCS);
	void update(float elapsedTime);
	void draw();
	void applyLighting(RenderVertex &targetVertex) const;

private:
	PlayState* playState;

	// Per particle: unit direction scaled by its own speed factor, so the ring
	// thickens slightly as it travels (cheaper and better looking than random
	// jitter each frame).
	float *directionX;
	float *directionY;
	int numParticles;

	NovaVertex originWCS;
	NovaVertex originCCS;
	float speed;
	float radius;
	float maxRadius;
	float brightness;
	bool active;

	NovaColor baseColor;
	NovaColor particleColor;
	NovaColor lightColor;

	// Squared band limits for a cheap reject in applyLighting (no sqrt).
	float innerRadiusSquared;
	float outerRadiusSquared;
};

#endif // __SMART_BOMB_WAVE_H
