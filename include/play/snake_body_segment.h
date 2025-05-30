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
#ifndef __SNAKE_BODY_SEGMENT_H
#define __SNAKE_BODY_SEGMENT_H

class SnakeBodySegment: public SnakeSegment {
public:
	SnakeBodySegment(PlayState* playState, const NovaColor &highColor);

	SnakeSegment* getParent() {
		return parent;
	}

	void setParent(SnakeSegment *parent) {
		this->parent = parent;
	}

	bool isDisabled() const {
		return disabled;
	}

	void setDisabled(bool disabled) {
		this->disabled = disabled;
	}

	// Overridden.
	void setActive(bool active);

	//void init(const NovaColor &highColor);

	// Overridden.
	void update(float elapsedTime);

	// Overridden.
	void youHit(Player *player);

	// Overridden.
	void youHit(Missile *missile);

protected:
	SnakeSegment *parent;
	bool disabled;
	NovaColor highColor;
	NovaColor lowColor;

};

#endif // __SNAKE_BODY_SEGMENT_H
