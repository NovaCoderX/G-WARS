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

#define RESPAWN_INTERVAL 33
#define CAMERA_ZOOM_FACTOR 10
#define CAMERA_MAX_ZOOM_AMOUNT -120
#define CAMERA_MIN_ZOOM_AMOUNT -240
#define TIME_DEMO_DURATION 100000

static Uint32 lastFrameTime = 0;

#ifdef TIME_DEMO_ENABLED
static Uint32 timeDemoEndTime = 0;
#endif

PlayState::PlayState() : GameState(PLAY_STATE) {
	camera = NULL;
	player = NULL;
	playAreaController = NULL;
	starfieldController = NULL;
	alienController = NULL;
	nuggetController = NULL;
	missileController = NULL;
	explosionController = NULL;
	hudController = NULL;
	resurrectionTimer = 0;
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
	NovaVertex newCameraPosition = camera->getPositionWCS();

	switch (keysym->sym) {
	case SDLK_RETURN:
		// Alt-enter toggles full screen mode
		if (keysym->mod & KMOD_ALT) {
			g_worldManager->toggleFullScreen();
		}
		break;

	case SDLK_EQUALS:
		newCameraPosition.z += CAMERA_ZOOM_FACTOR;
		if (newCameraPosition.z > CAMERA_MAX_ZOOM_AMOUNT) {
			newCameraPosition.z = CAMERA_MAX_ZOOM_AMOUNT;
		}

		camera->moveTo(newCameraPosition);
		camera->calculateWorldToCameraMatrix();
		break;
	case SDLK_MINUS:
		newCameraPosition.z -= CAMERA_ZOOM_FACTOR;
		if (newCameraPosition.z < CAMERA_MIN_ZOOM_AMOUNT) {
			newCameraPosition.z = CAMERA_MIN_ZOOM_AMOUNT;
		}

		camera->moveTo(newCameraPosition);
		camera->calculateWorldToCameraMatrix();
		break;
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
				if (player->getNumLives()) {
					player->fireMissileRequest(true);
				} else {
					// Game over man.
					if (hudController->getGameOverPanel()->isAnimationComplete()) {
						g_worldManager->setActiveState(MENU_STATE);
					}
				}
			}

			if (event.jbutton.button == 1) {
				if (player->isActive()) {
					player->dropBombRequest();
				}
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
	float elapsedTime = (currentFrameTime - lastFrameTime) / 100.0;

#ifdef TIME_DEMO_ENABLED
		if (currentFrameTime >= timeDemoEndTime) {
			logMessage("Time demo completed, shutting down...\n");
			exit(EXIT_SUCCESS);
		}
#else
		// First update the player.
		if (player->isActive()) {
			player->update(elapsedTime);
		} else if (player->getNumLives()) {
			// Player will be reborn....
			resurrectionTimer += elapsedTime;
			if (resurrectionTimer >= RESPAWN_INTERVAL) {
				// Back to life.
				player->setActive(true);

				// Reset.
				resurrectionTimer = 0;
			}
		}
#endif

	// Then the entity updates.
	missileController->update(elapsedTime);
	alienController->update(elapsedTime);
	nuggetController->update(elapsedTime);

	// See if any collisions resulted from the entity updates.
	missileController->checkCollisions();
	alienController->checkCollisions();
	nuggetController->checkCollisions();

	// Show the results from any collisions.
	explosionController->update(elapsedTime);
	playAreaController->update(elapsedTime);

	// Misc updates.
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

	if (player->isVisible()) {
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
#ifdef TIME_DEMO_ENABLED
		logMessage("Starting a time demo...\n");
		g_worldManager->setMusicType(NO_MUSIC);
		if (!player->isActive()) {
			player->setActive(true);
		}
#else
		g_worldManager->setMusicType(GAMEPLAY_MUSIC);
		if (player->isActive()) {
			// Only restart the music if there is a game in progress.
			g_worldManager->startMusic();
		}
#endif

	// Need to check if the playfield/starfield toggles were just changed in the options menu.
	playAreaController->syncOptions();
	starfieldController->syncOptions();

	// Clear any pending input requests (only needed when resuming a game).
	player->resetInputRequests();

	// Update to the current time.
	lastFrameTime = SDL_GetTicks();

#ifdef TIME_DEMO_ENABLED
	timeDemoEndTime = (lastFrameTime + TIME_DEMO_DURATION);
#endif
}

void PlayState::leaveState() {
	// Stop playing the gameplay music.
	g_worldManager->stopMusic();
}

