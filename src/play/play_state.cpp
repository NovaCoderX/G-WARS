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
#include "poly_nova.h"


static Uint32 lastFrameTime = 0;

PlayState::PlayState() {
	camera = NULL;
	player = NULL;
	playAreaController = NULL;
	starfieldController = NULL;
	alienController = NULL;
	nuggetController = NULL;
	missileController = NULL;
	explosionController = NULL;
	hudController = NULL;
}

PlayState::~PlayState() {
	if (camera) {
		delete camera;
		camera = NULL;
	}

	if (player) {
		delete player;
		player = NULL;
	}

	if (playAreaController) {
		delete playAreaController;
		playAreaController = NULL;
	}

	if (starfieldController) {
		delete starfieldController;
		starfieldController = NULL;
	}

	if (alienController) {
		delete alienController;
		alienController = NULL;
	}

	if (nuggetController) {
		delete nuggetController;
		nuggetController = NULL;
	}

	if (missileController) {
		delete missileController;
		missileController = NULL;
	}

	if (explosionController) {
		delete explosionController;
		explosionController = NULL;
	}

	if (hudController) {
		delete hudController;
		hudController = NULL;
	}
}

void PlayState::init() {
	camera = new Camera();
	player = new Player(this);

	playAreaController = new PlayAreaController(this);
	playAreaController->init();

	starfieldController = new StarfieldController(this);
	starfieldController->init();

	alienController = new AlienController(this);
	alienController->init();

	nuggetController = new NuggetController(this);
	nuggetController->init();

	missileController = new MissileController(this);
	missileController->init();

	explosionController = new ExplosionController(this);
	explosionController->init();

	hudController = new HudController(this);
	hudController->init();
}

void PlayState::handleKeyDown(SDL_keysym *keysym) {
	switch (keysym->sym) {
	case SDLK_ESCAPE:
		g_worldManager->setActiveState(MENU_STATE);
		break;
	default:
		break;
	}
}

void PlayState::handleKeyUp(SDL_keysym *keysym) {
	// Empty.
}

void PlayState::processInput() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_JOYAXISMOTION:
			if (event.jaxis.axis == 0) {
				// Horizontal movement.
				if (event.jaxis.value < 0) {
					player->moveLeftRequest(true);
				} else if (event.jaxis.value > 0) {
					player->moveRightRequest(true);
				} else {
					// Returned to the center.
					player->moveLeftRequest(false);
					player->moveRightRequest(false);
				}
			} else {
				// Vertical movement.
				if (event.jaxis.value < 0) {
					player->moveUpRequest(true);
				} else if (event.jaxis.value > 0) {
					player->moveDownRequest(true);
				} else {
					// Returned to the center.
					player->moveUpRequest(false);
					player->moveDownRequest(false);
				}
			}
			break;
		case SDL_JOYBUTTONDOWN:
			if (event.jbutton.button == 0) {
				player->fireMissileRequest(true);
			}

			if (event.jbutton.button == 1) {
				player->dropBombRequest();
			}
			break;
		case SDL_JOYBUTTONUP:
			if (event.jbutton.button == 0) {
				player->fireMissileRequest(false);
			}
			break;

		case SDL_KEYDOWN:
			handleKeyDown(&event.key.keysym);
			break;
		case SDL_KEYUP:
			handleKeyUp(&event.key.keysym);
			break;
		case SDL_QUIT:
			exit(EXIT_SUCCESS);
			break;
		}
	}
}

void PlayState::update() {
	Uint32 currentFrameTime = SDL_GetTicks();
	float elapsedTime = (currentFrameTime - lastFrameTime) / 1000.0;

	player->update(elapsedTime);
	missileController->update(elapsedTime);
	alienController->update(elapsedTime);
	nuggetController->update(elapsedTime);
	missileController->checkCollisions();
	alienController->checkCollisions();
	nuggetController->checkCollisions();
	explosionController->update(elapsedTime);
	playAreaController->update(elapsedTime);
	starfieldController->update(elapsedTime);
	hudController->update(elapsedTime);

	// Store.
	lastFrameTime = currentFrameTime;
}

void PlayState::draw() {
	// Must draw stars separately.
	glBegin(GL_POINTS);
	starfieldController->draw();
	glEnd();

	// Draw all line based objects.
	glBegin(GL_LINES);
	playAreaController->draw();

	// If the player is active then it is also visible (implied).
	if (player->isActive()) {
		player->draw();
	}

	missileController->draw();
	alienController->draw();
	nuggetController->draw();
	hudController->draw();
	glEnd();

	// Must draw particles separately.
	glBegin(GL_POINTS);
	explosionController->draw();
	glEnd();
}

void PlayState::reset() {
	if (camera) {
		delete camera;
		camera = NULL;
	}

	if (player) {
		delete player;
		player = NULL;
	}

	if (playAreaController) {
		delete playAreaController;
		playAreaController = NULL;
	}

	if (starfieldController) {
		delete starfieldController;
		starfieldController = NULL;
	}

	if (alienController) {
		delete alienController;
		alienController = NULL;
	}

	if (nuggetController) {
		delete nuggetController;
		nuggetController = NULL;
	}

	if (missileController) {
		delete missileController;
		missileController = NULL;
	}

	if (explosionController) {
		delete explosionController;
		explosionController = NULL;
	}

	if (hudController) {
		delete hudController;
		hudController = NULL;
	}

	this->init();
}

void PlayState::enterState() {
	if (this->getPlayer()->isActive()) {
		// Only resume the music if there is a game in progress.
		g_worldManager->resumeMusic();
	}

	// Update to the current time.
	lastFrameTime = SDL_GetTicks();
}

void PlayState::leaveState() {
	if (this->getPlayer()->isActive()) {
		// Only pause the music if there is a game in progress.
		g_worldManager->pauseMusic();
	}
}

