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
#ifndef __NUGGET_H
#define __NUGGET_H

// Forward declaration.
class Player;

enum NuggetType {
	MULTIPLIER_NUGGET = 0,
	POWER_UP_NUGGET = 1,
	EXTRA_LIFE_NUGGET = 2,
	EXTRA_BOMB_NUGGET = 3
};

class Nugget: public Sprite {
public:
	Nugget(PlayState* playState, NuggetType nuggetType);

	// Overridden.
	void setActive(bool active);

	NuggetType getNuggetType() const {
		return nuggetType;
	}

	const NovaColor& getDefaultColor() const {
		return defaultColor;
	}

	void setDefaultColor(const NovaColor &color) {
		this->defaultColor = color;
	}

	void checkCollision(Player *player);

public:
	Nugget *nextInList;
	Nugget *priorInList;

protected:
	PlayState* playState;
	NuggetType nuggetType;

private:
	NovaColor defaultColor;
};

#endif // __NUGGET_H
