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
#ifndef __JELLY_H
#define __JELLY_H

// Forward declaration.
class Tentacle;

class Jelly: public Alien {
public:
	Jelly(PlayState* playState);
	~Jelly();

	enum AnchorPoint {
		FIRST_TENTACLE_ANCHOR = 0, SECOND_TENTACLE_ANCHOR = 1, THIRD_TENTACLE_ANCHOR = 2, FOURTH_TENTACLE_ANCHOR = 3
	};

	NovaVertex getAnchorPointWCS(AnchorPoint anchor) const;

	// Overridden.
	void setActive(bool active);

	// Overridden.
	const NovaColor& getExplosionColor() const {
		return highColor;
	}

	// Overridden.
	void update(float elapsedTime);

	// Overridden.
	bool checkCollision(Player* player);

	// Overridden.
	bool checkCollision(Missile* missile);

	// Overridden
	void smartBombNotification();

	// Overridden.
	void draw();

private:
	Sprite* highlight;
	std::vector<Tentacle*> tentacles;
	NovaColor highColor;
	NovaColor lowColor;
	bool increasingColor;
};

#endif // __JELLY_H
