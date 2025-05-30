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
	UNDEFINED_ALIEN = 0,
	BOUNCE_CUBE = 1,
	BOUNCE_WANDERER = 2,
	BOUNCE_STAR = 3,
	BOUNCE_HEXAGON = 4,
	CHASE_RHOMBUS = 5,
	CHASE_STAR = 6,
	CHASE_SHURIKEN = 7,
	CHASE_CONCAVE = 8,
	ATTACK_TANK = 9,
	ATTACK_SHIP = 10,
	ATTACK_ARTILLERY = 11,
	ATTACK_NEUTRON = 12,
	ROCKET_SHIP = 13,
	FLYING_SAUCER = 14,
	PLAYER_CLONE = 15,
	BLACK_HOLE = 16,
	MINI_GATE = 17,
	SNAKE = 18
};

// Forward declaration.
class Player;

class Alien: public Sprite {
	friend class AlienController;
	friend class MissileController;
public:
	Alien(PlayState* playState);

	AlienType getAlienType() const {
		return alienType;
	}

	// Overridden.
	void setActive(bool active);

	virtual void checkCollision(Player *player);
	virtual void checkCollision(Missile *missile);

protected:
	PlayState* playState;
	AlienType alienType;
	Alien *nextInList;
	Alien *priorInList;
};

#endif // __ALIEN_H
