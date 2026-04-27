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
#ifndef __ALIEN_COMPONENT_H
#define __ALIEN_COMPONENT_H

class AlienComponent : public Sprite, public ExplosiveObject {
public:
	AlienComponent(PlayState* playState);

	PlayState* getPlayState() {
		return playState;
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
		return activeColor;
	}

	const NovaColor& getActiveColor() const {
		return activeColor;
	}

	void setActiveColor(const NovaColor &color) {
		this->activeColor = color;
	}

	const NovaColor& getInactiveColor() const {
		return inactiveColor;
	}

	void setInactiveColor(const NovaColor &color) {
		this->inactiveColor = color;
	}

	virtual void update(float elapsedTime) {}
	virtual bool checkCollision(Player* player);
	virtual bool checkCollision(Missile* missile);
	virtual void smartBombNotification() {}

protected:
	PlayState* playState;
	bool active;
	NovaColor activeColor;
	NovaColor inactiveColor;
};

#endif // __ALIEN_COMPONENT_H
