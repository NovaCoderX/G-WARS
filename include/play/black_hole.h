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
#ifndef __BLACK_HOLE_H
#define __BLACK_HOLE_H

class BlackHoleRing: public AlienComponent {
public:
	BlackHoleRing(PlayState* playState, Alien* parent, SpriteDefinition *definition, const NovaColor &color);

	// Overridden.
	void update(float elapsedTime);

private:
	NovaColor highColor;
	NovaColor lowColor;
	bool increasingColor;
};

class BlackHole: public Alien {
public:
	BlackHole(PlayState* playState);
	~BlackHole();

	// Overridden.
	void setActive(bool active);

	// Overridden.
	void update(float elapsedTime);

	// Overridden.
	bool checkCollision(Player *player);

	// Overridden.
	bool checkCollision(Missile *missile);

	// Overridden.
	void draw();

	void applyGravitionalPull(Player *player);

private:
	SpriteDefinition *innerRingDefinition; // Definition used by this parent object.
	SpriteDefinition *outerRingDefinition;  // Definition used by the child object.

	// Child object.
	BlackHoleRing *outerRing;
};

#endif // __BLACK_HOLE_H
