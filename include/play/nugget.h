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

class Nugget: public Sprite {
	friend class NuggetController;
public:
	Nugget(PlayState* playState);

	enum NuggetType {
		UNDEFINED = 0,
		MULTIPLIER = 1,
		POWER_UP = 2
	};

	NuggetType getNuggetType() const {
		return nuggetType;
	}

	void checkCollision(Player *player);

protected:
	PlayState* playState;
	NuggetType nuggetType;
	Nugget *nextInList;
	Nugget *priorInList;
};

#endif // __NUGGET_H
