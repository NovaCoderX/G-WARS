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

#define TITLE_VERTICAL_POSITION 50
#define FIRST_CONTROL_VERTICAL_POSITION 31
#define FIRST_BUTTON_VERTICAL_POSITION -50
#define CONTROL_VERTICAL_SPACING 8.8
#define PANEL_VERTICAL_POSITION 0
#define PANEL_WIDTH 110
#define PANEL_HEIGHT 72

enum MenuControls {
	PLAYER_NAME_INPUT = 0,
	ONLINE_SAVING_TOGGLE = 1,
	SOUND_VOLUME_INPUT = 2,
	MUSIC_VOLUME_INPUT = 3,
	MIXER_QUALITY_INPUT = 4,
	MIXER_CHANNELS_INPUT = 5,
	PLAYFIELD_TOGGLE = 6,
	STARFIELD_TOGGLE = 7,
	RETURN_BUTTON = 8
};

OptionsMenu::OptionsMenu(MenuState* menuState) : BaseMenu(menuState) {
	title = NULL;
	panel = NULL;
	currentSelection = RETURN_BUTTON;
}

OptionsMenu::~OptionsMenu() {
	if (title) {
		delete title;
		title = NULL;
	}

	if (panel) {
		delete panel;
		panel = NULL;
	}

    for (Control* control : controls) {
        delete control;
    }

    controls.clear();

    for (Label* label : labels) {
        delete label;
    }

    labels.clear();
}

void OptionsMenu::init() {
	logMessage("Creating the options menu...\n");

	float x = 0;
	float y = TITLE_VERTICAL_POSITION;
	float z = 0;
	NovaColor titleColor = DEFAULT_TEXT_COLOR;
	title = new MenuTitle(menuState, "OPTIONS", titleColor, NovaVertex(x, y, z));

	// Setup the background panel.
	NovaColor panelColor = DEFAULT_TEXT_COLOR;
	panelColor.rebase(20);
	NovaColor borderColor = DEFAULT_TEXT_COLOR;
	panel = new MenuPanel(menuState, PANEL_WIDTH, PANEL_HEIGHT, panelColor, borderColor, NovaVertex(0, PANEL_VERTICAL_POSITION, 0));

	// Setup the controls.
	x = SMALL_CHARACTER_WIDTH;
	NovaColor textColor = DEFAULT_TEXT_COLOR;
	y = FIRST_CONTROL_VERTICAL_POSITION;

	controls.push_back(new OptionTextEdit(menuState, "player.name", DEFAULT_PLAYER_NAME, MAX_PLAYER_NAME_LENGTH, textColor, NovaVertex(x, y, z)));
	labels.push_back(new Label(menuState, "PLAYER NAME:", textColor, RIGHT_ALIGNED_LABEL, NovaVertex(x, y, z)));
	y -= CONTROL_VERTICAL_SPACING;

	controls.push_back(new OptionToggle(menuState, "highscores.online", true, textColor, NovaVertex(x, y, z)));
	labels.push_back(new Label(menuState, "ONLINE:", textColor, RIGHT_ALIGNED_LABEL, NovaVertex(x, y, z)));
	y -= CONTROL_VERTICAL_SPACING;

	std::vector<int> volume = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	controls.push_back(new OptionSelector(menuState, "audio.sound.volume", DEFAULT_SOUND_VOLUME, volume, textColor, NovaVertex(x, y, z)));
	labels.push_back(new Label(menuState, "FX VOLUME:", textColor, RIGHT_ALIGNED_LABEL, NovaVertex(x, y, z)));
	y -= CONTROL_VERTICAL_SPACING;

	controls.push_back(new OptionSelector(menuState, "audio.music.volume", DEFAULT_MUSIC_VOLUME, volume, textColor, NovaVertex(x, y, z)));
	labels.push_back(new Label(menuState, "MUSIC VOLUME:", textColor, RIGHT_ALIGNED_LABEL, NovaVertex(x, y, z)));
	y -= CONTROL_VERTICAL_SPACING;

	std::vector<int> frequencies = { 11025, 22050, 44100 };
	controls.push_back(new OptionSelector(menuState, "audio.mixer.quality", MIX_DEFAULT_FREQUENCY, frequencies, textColor, NovaVertex(x, y, z)));
	labels.push_back(new Label(menuState, "MIX QUALITY:", textColor, RIGHT_ALIGNED_LABEL, NovaVertex(x, y, z)));
	y -= CONTROL_VERTICAL_SPACING;

	std::vector<int> channels = { 16, 32, 64 };
	controls.push_back(new OptionSelector(menuState, "audio.mixer.channels", DEFAULT_MIXER_CHANNELS, channels, textColor, NovaVertex(x, y, z)));
	labels.push_back(new Label(menuState, "MIX CHANNELS:", textColor, RIGHT_ALIGNED_LABEL, NovaVertex(x, y, z)));
	y -= CONTROL_VERTICAL_SPACING;

	controls.push_back(new OptionToggle(menuState, "playfield.enabled", true, textColor, NovaVertex(x, y, z)));
	labels.push_back(new Label(menuState, "PLAYFIELD:", textColor, RIGHT_ALIGNED_LABEL, NovaVertex(x, y, z)));
	y -= CONTROL_VERTICAL_SPACING;

	controls.push_back(new OptionToggle(menuState, "starfield.enabled", true, textColor, NovaVertex(x, y, z)));
	labels.push_back(new Label(menuState, "STARFIELD:", textColor, RIGHT_ALIGNED_LABEL, NovaVertex(x, y, z)));

	x = 0;
	y = FIRST_BUTTON_VERTICAL_POSITION;
	NovaColor buttonColor = NovaColor(101, 17, 140);
	textColor = NovaColor(255, 255, 255);
	controls.push_back(new Button(menuState, buttonColor, "RETURN", textColor, NovaVertex(x, y, z)));
}

void OptionsMenu::syncCurrentSelection() {
	// Sync the controls.
	for (Control* control : controls) {
		control->setSelected(false);
	}

	controls[currentSelection]->setSelected(true);
}

void OptionsMenu::handleKeyDown(SDL_keysym *keysym) {
	bool shortcutKeyUsed = false;

	// Alt-enter toggles full screen mode
	if ((keysym->sym == SDLK_RETURN) && (keysym->mod & KMOD_ALT)) {
		g_worldManager->toggleFullScreen();
		shortcutKeyUsed = true;
	}

	if (!shortcutKeyUsed) {
		// Normal processing.
		switch (keysym->sym) {
		case SDLK_ESCAPE:
			currentSelection = RETURN_BUTTON;
			syncCurrentSelection();
			break;
		case SDLK_UP:
			selectionUp();
			break;
		case SDLK_DOWN:
			selectionDown();
			break;
		case SDLK_RETURN:
			selectionChosen();
			break;
		default:
			break;
		}
	}
}

void OptionsMenu::handleKeyUp(SDL_keysym *keysym) {
	// Empty.
}

void OptionsMenu::processInput() {
	if (controls[currentSelection]->isInputCaptured()) {
		controls[currentSelection]->processInput();
	} else {
		// Let the menu process the input.
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_JOYAXISMOTION:
				if (event.jaxis.axis == 1) {
					// Vertical movement.
					if (event.jaxis.value < 0) {
						selectionUp();
					} else if (event.jaxis.value > 0) {
						selectionDown();
					}
				}
				break;
			case SDL_JOYBUTTONDOWN:
				if (event.jbutton.button == 0) {
					selectionChosen();
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
}

void OptionsMenu::update(float elapsedTime) {
	for (Control* control : controls) {
		control->update(elapsedTime);
	}
}

void OptionsMenu::draw() {
	title->draw();
	panel->draw();

	for (Control* control : controls) {
		control->draw();
	}

	glBegin(GL_LINES);
	for (Label* label : labels) {
		label->draw();
	}
	glEnd();
}

void OptionsMenu::reset() {
	currentSelection = 0;
}

void OptionsMenu::enterState() {
	// Update the selected controls.
	currentSelection = RETURN_BUTTON;
	syncCurrentSelection();

	// Create a record of all the current [audio] settings from YAML.
	audioData.soundVolume = g_worldManager->getYamlish()->getInt("audio.sound.volume");
	audioData.musicVolume = g_worldManager->getYamlish()->getInt("audio.music.volume");
	audioData.mixerQuality = g_worldManager->getYamlish()->getInt("audio.mixer.quality");
	audioData.mixerChannels = g_worldManager->getYamlish()->getInt("audio.mixer.channels");
}

void OptionsMenu::leaveState() {
	// See if the audio settings in the YAML were changed, if so we will need to restart the audio manager.
	bool audioRestartRequired = false;

	if (audioData.soundVolume != g_worldManager->getYamlish()->getInt("audio.sound.volume")) {
		audioRestartRequired = true;
	}

	if (audioData.musicVolume != g_worldManager->getYamlish()->getInt("audio.music.volume")) {
		audioRestartRequired = true;
	}

	if (audioData.mixerQuality != g_worldManager->getYamlish()->getInt("audio.mixer.quality")) {
		audioRestartRequired = true;
	}

	if (audioData.mixerChannels != g_worldManager->getYamlish()->getInt("audio.mixer.channels")) {
		audioRestartRequired = true;
	}

	if (audioRestartRequired) {
		g_worldManager->restartAudioManager();
	}

	// Need to check if the player's name was just changed.
	g_worldManager->getHighScoreHandler()->syncOptions();
}

void OptionsMenu::selectionUp() {
	g_worldManager->startSound(UI_CHANGE);
	currentSelection--;
	if (currentSelection < PLAYER_NAME_INPUT ) {
		currentSelection = RETURN_BUTTON ;
	}

	// If the control is disabled we should skip it.
	if (controls[currentSelection]->isDisabled()) {
		currentSelection--;
	}

	syncCurrentSelection();
}

void OptionsMenu::selectionDown() {
	g_worldManager->startSound(UI_CHANGE);
	currentSelection++;
	if (currentSelection > RETURN_BUTTON) {
		currentSelection = PLAYER_NAME_INPUT ;
	}

	// If the control is disabled we should skip it.
	if (controls[currentSelection]->isDisabled()) {
		currentSelection++;
	}

	syncCurrentSelection();
}

void OptionsMenu::selectionChosen() {
	g_worldManager->startSound(UI_CONFIRM);
	if (currentSelection == RETURN_BUTTON) {
		menuState->setActiveMenu(MAIN_MENU);
	} else {
		controls[currentSelection]->setInputCaptured(true);
	}
}

