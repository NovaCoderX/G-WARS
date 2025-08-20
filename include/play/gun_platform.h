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
#ifndef __GUN_PLATFORM_H
#define __GUN_PLATFORM_H

class GunPlatform: public AlienComponent {
public:
	GunPlatform(PlayState* playState, Sprite* parent, int anchorIndex);
	~GunPlatform();

	// Overridden.
	void setDisabled(bool disabled);

	// Overridden.
	void setActive(bool active);

	// Overridden.
	void update(float elapsedTime);

	// Overridden.
	bool checkCollision(Missile* missile);

	// Overridden.
	void draw();

private:
	int anchorIndex;
	Sprite* gunTurret;
	NovaColor defaultPlatformColor;
	NovaColor defaultGunColor;
	NovaColor disabledColor;
	float damage;
	float lastFireTime;
	float totalElapsedTime;
};

#endif // __GUN_PLATFORM_H
