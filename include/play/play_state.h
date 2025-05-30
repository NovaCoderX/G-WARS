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
#ifndef __PLAY_STATE_H
#define __PLAY_STATE_H

class PlayState : public GameState
{
public:
	PlayState();
	~PlayState();

	bool isGameOver()  {
		// Or not started yet!
		bool gameOver = true;

		if (player) {
			gameOver = (!player->getNumLives());
		}

		return gameOver;
	}

	Camera* getCamera() const {
		return camera;
	}

	Player* getPlayer() const {
		return player;
	}

	PlayAreaController* getPlayAreaController() const {
		return playAreaController;
	}

	StarfieldController* getStarfieldController() const {
		return starfieldController;
	}

	MissileController* getMissileController() const {
		return missileController;
	}

	AlienController* getAlienController() const {
		return alienController;
	}

	NuggetController* getNuggetController() const {
		return nuggetController;
	}

	ExplosionController* getExplosionController() const {
		return explosionController;
	}

	HudController* getHudController() const {
		return hudController;
	}

	void init();
	void processInput();
	void update();
	void draw();
	void reset();
	void enterState();
	void leaveState();

private:
	Camera *camera;
	Player* player;
	PlayAreaController* playAreaController;
	StarfieldController* starfieldController;
	AlienController* alienController;
	NuggetController* nuggetController;
	MissileController* missileController;
	ExplosionController* explosionController;
	HudController* hudController;

	void handleKeyDown(SDL_keysym *keysym);
	void handleKeyUp(SDL_keysym *keysym);
};

#endif  // __PLAY_STATE_H
