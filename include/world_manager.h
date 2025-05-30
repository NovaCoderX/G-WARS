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
#ifndef __WORLD_MANAGER_H
#define __WORLD_MANAGER_H

enum StateType {
	MENU_STATE = 0,
	PLAY_STATE = 1
};

class WorldManager {
public:
	WorldManager();
	~WorldManager();

	const std::string& getBaseDirectory() const {
		return baseDirectory;
	}

	void startSound(SampleType sampleType) {
		audioManager->startSound(sampleType);
	}

	void setMusicType(MusicType musicType) {
		audioManager->setMusicType(musicType);
	}

	void startMusic(bool fadeIn = true) {
		audioManager->startMusic(fadeIn);
	}

	void stopMusic(bool fadeOut = true) {
		audioManager->stopMusic(fadeOut);
	}

	void pauseMusic() {
		audioManager->pauseMusic();
	}

	void resumeMusic() {
		audioManager->resumeMusic();
	}

	void restartAudioManager();

	SpriteDefinition* getSpriteDefinition(const char *name) const {
		return spriteDefinitionLibrary->getDefinition(name);
	}

	Yamlish* getYamlish() const {
		return yamlish;
	}

	HighScoreHandler* getHighScoreHandler() const {
		return highScoreHandler;
	}

	void setActiveState(StateType state, bool reset = false);

	bool isGameOver() {
		// Or not started yet!
	    bool gameOver = true;

	    if (playState) {
	        gameOver = playState->isGameOver();
	    }

	    return gameOver;
	}

	void load(const char *baseDirectory);
	void save(); // save the world.
	void run();

private:
	std::string baseDirectory;
	DisplayManager* displayManager;
	AudioManager* audioManager;
	InputManager* inputManager;
	SpriteDefinitionLibrary* spriteDefinitionLibrary;
	Yamlish* yamlish;
	HighScoreHandler* highScoreHandler;
	GameState* activeState;
	MenuState* menuState;
	PlayState* playState;
};

#endif // __WORLD_MANAGER_H

