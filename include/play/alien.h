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
#ifndef __ALIEN_H
#define __ALIEN_H

enum AlienType {
	BOUNCE_CUBE_ALIEN = 0,
	BOUNCE_WANDERER_ALIEN = 1,
	BOUNCE_STAR_ALIEN = 2,
	BOUNCE_HEXAGON_ALIEN = 3,
	CHASE_RHOMBUS_ALIEN = 4,
	CHASE_STAR_ALIEN = 5,
	CHASE_SHURIKEN_ALIEN = 6,
	CHASE_CONCAVE_ALIEN = 7,
	ATTACK_TANK_ALIEN = 8,
	ATTACK_SHIP_ALIEN = 9,
	ATTACK_ARTILLERY_ALIEN = 10,
	ATTACK_NEUTRON_ALIEN = 11,
	ROCKET_SHIP_ALIEN = 12,
	FLYING_SAUCER_ALIEN = 13,
	PLAYER_CLONE_ALIEN = 14,
	BLACK_HOLE_ALIEN = 15,
	MINI_GATE_ALIEN = 16,
	MINI_CRUSHER_ALIEN = 17,
	SNAKE_ALIEN = 18,
	JELLY_ALIEN = 19,
	CRUSHER_ALIEN = 20
};

class Alien: public Sprite, public ExplosiveObject {
public:
	Alien(PlayState* playState, AlienType alienType);

	// Overridden.
	void setActive(bool active);

	PlayState* getPlayState() {
		return playState;
	}

	AlienType getAlienType() const {
		return alienType;
	}

	enum AlienCategory {
		STANDARD_ALIEN = 0,
		SPECIAL_ALIEN = 1,
		BOSS_ALIEN = 2
	};

	AlienCategory getAlienCategory() const {
		return alienCategory;
	}

	const NovaColor& getDefaultColor() const {
		return defaultColor;
	}

	void setDefaultColor(const NovaColor &color) {
		this->defaultColor = color;
	}

	// Overridden.
	NovaVertex getExplosionOrigin() const {
		return this->getPositionWCS();
	}

	// Overridden.
	const NovaColor& getExplosionColor() const {
		return currentColor;
	}

	virtual bool checkCollision(Player* player);
	virtual bool checkCollision(Missile* missile);
	virtual void smartBombNotification();

public:
	Alien* nextInList;
	Alien* priorInList;

protected:
	PlayState* playState;

private:
	AlienType alienType;
	AlienCategory alienCategory;
	NovaColor defaultColor;
};

#endif // __ALIEN_H
