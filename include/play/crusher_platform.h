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
#ifndef __CRUSHER_PLATFORM_H
#define __CRUSHER_PLATFORM_H

class CrusherPlatform: public AlienComponent {
public:
	CrusherPlatform(PlayState* playState, Sprite* parent);
	~CrusherPlatform();

	// Overridden.
	void setDisabled(bool disabled);

	// Overridden.
	void setActive(bool active);

	// Overridden.
	void update(float elapsedTime);

	// Overridden.
	bool checkCollision(Player* player);

	// Overridden.
	bool checkCollision(Missile* missile);

	// Overridden.
	void draw();

protected:
	std::vector<AlienComponent*> components;
	NovaColor defaultColor;
	NovaColor disabledColor;
};

class LeftCrusherPlatform: public CrusherPlatform {
public:
	LeftCrusherPlatform(PlayState* playState, Sprite* parent);

	// Overridden.
	void setActive(bool active);

	// Overridden.
	void update(float elapsedTime);
};

class RightCrusherPlatform: public CrusherPlatform {
public:
	RightCrusherPlatform(PlayState* playState, Sprite* parent);

	// Overridden.
	void setActive(bool active);

	// Overridden.
	void update(float elapsedTime);
};



#endif // __CRUSHER_PLATFORM_H
