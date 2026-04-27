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

enum AlienCategory {
	STANDARD_ALIEN = 0,
	SPECIAL_ALIEN = 1,
	BOSS_ALIEN = 2
};

class Alien: public Sprite, public ExplosiveObject {
public:
	Alien(PlayState* playState, AlienCategory alienCategory = STANDARD_ALIEN);

	PlayState* getPlayState() {
		return playState;
	}

	AlienCategory getAlienCategory() const {
		return alienCategory;
	}

	bool isActive() const {
		return active;
	}

	virtual void setActive(bool active);

	// Overridden.
	NovaVertex getExplosionOrigin() const {
		return this->getPositionWCS();
	}

	// Overridden.
	const NovaColor& getExplosionColor() const {
		return currentColor;
	}

	virtual uint getNumberAwardPoints() = 0;

	virtual void update(float elapsedTime) {}
	virtual bool checkCollision(Player* player);
	virtual bool checkCollision(Missile* missile);
	virtual void smartBombNotification();
	virtual void miniGateNotification();

public:
	Alien* nextInList;
	Alien* priorInList;

protected:
	PlayState* playState;
	AlienCategory alienCategory;
	bool active;
};

#endif // __ALIEN_H
