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

class AlienComponent : public Sprite {
public:
	AlienComponent(PlayState* playState, Sprite* parent);

	bool isVulnerable() const {
		return vulnerable;
	}

	virtual void setVulnerable(bool vulnerable) {
		this->vulnerable = vulnerable;
	}

	bool isDisabled() const {
		return disabled;
	}

	virtual void setDisabled(bool disabled) {
		this->disabled = disabled;
	}

	ExplosionSize getExplosionSize() const {
		return explosionSize;
	}

	void setExplosionSize(ExplosionSize size) {
		this->explosionSize = size;
	}

	const NovaColor& getExplosionColor() const {
		return explosionColor;
	}

	void setExplosionColor(const NovaColor& explosionColor) {
		this->explosionColor = explosionColor;
	}

	// Overridden.
	void setActive(bool active);

	virtual bool checkCollision(Player* player);
	virtual bool checkCollision(Missile* missile);

protected:
	PlayState* playState;
	Sprite* parent;
	bool vulnerable;
	bool disabled;
	ExplosionSize explosionSize;
	NovaColor explosionColor;
};

#endif // __ALIEN_COMPONENT_H
