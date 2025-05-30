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
#ifndef __MiSSILE_CONTROLLER_H
#define __MiSSILE_CONTROLLER_H

class MissileController {
public:
	MissileController(PlayState* playState);
	~MissileController();

	void init();
	void launchMissile(const Player *player);
	void launchMissile(const Alien *alien, double direction, const NovaVertex& origin);
	void update(float elapsedTime);
	void checkCollisions();
	void draw();
	void deactivate(Missile *missile);

private:
	PlayState* playState;
	Missile *missileListHead;
	std::vector<PlayerMissile*> playerMissiles;
	float additionalPlayerMissileVelocity;
	std::vector<AlienMissile*> alienMissiles;
	float additionalAlienMissileVelocity;

	void addToList(Missile *missile);
	void removeFromList(Missile *missile);
};

#endif // __MiSSILE_CONTROLLER_H
