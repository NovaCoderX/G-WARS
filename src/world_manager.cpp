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


WorldManager::WorldManager() {
	displayManager = NULL;
	audioManager = NULL;
	inputManager = NULL;
	spriteDefinitionLibrary = NULL;
	yamlish = NULL;
	highScoreHandler = NULL;
	activeState = NULL;
	menuState = NULL;
	playState = NULL;
}

WorldManager::~WorldManager() {
	logMessage("Shutting down the WorldManager.....\n");

	if (menuState) {
		delete menuState;
		menuState = NULL;
	}

	if (playState) {
		delete playState;
		playState = NULL;
	}

	if (spriteDefinitionLibrary) {
		delete spriteDefinitionLibrary;
		spriteDefinitionLibrary = NULL;
	}

	if (yamlish) {
		delete yamlish;
		yamlish = NULL;
	}

	if (highScoreHandler) {
		delete highScoreHandler;
		highScoreHandler = NULL;
	}

	// Release SDL resources last.
	if (inputManager) {
		delete inputManager;
		inputManager = NULL;
	}

	if (audioManager) {
		delete audioManager;
		audioManager = NULL;
	}

	if (displayManager) {
		delete displayManager;
		displayManager = NULL;
	}

	SDL_Quit();
}

void WorldManager::restartAudioManager() {
	logMessage("Restarting the audio manager...\n");
	if (audioManager) {
		delete audioManager;
		audioManager = NULL;
	}

	audioManager = new AudioManager();
	audioManager->init();
}

void WorldManager::setActiveState(StateType state, bool reset) {
	if (activeState) {
		activeState->leaveState();
	}

    if (state == PLAY_STATE) {
        if (!playState) {
            playState = new PlayState();
            playState->init();
            reset = false; // Because it was just reset.
        }

        activeState = playState;
        if (reset) {
             // Best way to reset a game in progress is just blow it all away and start again.
             // This means deleting the player, camera and all the controllers then recreating.
	        activeState->reset();
	    }
    }

    if (state == MENU_STATE) {
        if (!menuState) {
            menuState = new MenuState();
            menuState->init();
            reset = false; // Because it was just reset.
        }

        activeState = menuState;
        if (reset) {
	        activeState->reset();
	    }
    }

    if (activeState) {
    	activeState->enterState();
    }
}

void WorldManager::load(const char *baseDirectory) {
	logMessage("Creating the WorldManager.....\n");

	// Store the base directory.
	this->baseDirectory = baseDirectory;

	// Load the properties file.
	yamlish = new Yamlish();
	if (!yamlish->load("G-WARS.yaml")) {
		logWarningMessage("Could not load the properties file\n");
	}

	highScoreHandler = new HighScoreHandler();
	if (!highScoreHandler->load("HighScores.txt")) {
		logWarningMessage("Could not load the high score data\n");
	}

	if (SDL_Init(SDL_INIT_NOPARACHUTE) == -1) {
		fatalError("Could not initialize SDL: %s\n", SDL_GetError());
	}

	logMessage("Setting up the SDL display manager\n");
	displayManager = new DisplayManager();
	displayManager->init();

	logMessage("Setting up the SDL audio manager\n");
	audioManager = new AudioManager();
	audioManager->init();

	logMessage("Setting up the SDL input manager\n");
	inputManager = new InputManager();
	inputManager->init();

	// Must load the sprite definitions before we create any sprites.
	logMessage("Loading the sprite definition library\n");
	spriteDefinitionLibrary = new SpriteDefinitionLibrary();
	spriteDefinitionLibrary->load("G-WARS.scn");

	// This will create the menu state for the first time.
	this->setActiveState(MENU_STATE);

	logMessage("Finished creating the WorldManager\n");
}

void WorldManager::save() {
	if (yamlish) {
		yamlish->save("G-WARS.yaml");
	}

	if (highScoreHandler) {
		highScoreHandler->save("HighScores.txt");
	}
}

void WorldManager::run() {
	while (true) {
		activeState->processInput();
		activeState->update();

	    displayManager->begin();
		activeState->draw();
		displayManager->end();
	}
}


