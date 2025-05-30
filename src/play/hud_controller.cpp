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

HudController::HudController(PlayState* playState) {
	this->playState = playState;
	currentScorePanel = NULL;
	highScorePanel = NULL;
	playerStatsPanel = NULL;
	gameOverPanel = NULL;
}

HudController::~HudController() {
	if (currentScorePanel) {
		delete currentScorePanel;
		currentScorePanel = NULL;
	}

	if (highScorePanel) {
		delete highScorePanel;
		highScorePanel = NULL;
	}

	if (playerStatsPanel) {
		delete playerStatsPanel;
		playerStatsPanel = NULL;
	}

	if (gameOverPanel) {
		delete gameOverPanel;
		gameOverPanel = NULL;
	}
}

void HudController::init() {
	currentScorePanel = new CurrentScorePanel(playState);
	currentScorePanel->init();
	currentScorePanel->setDirty(true);

	highScorePanel = new HighScorePanel(playState);
	highScorePanel->init();
	highScorePanel->setDirty(true);

	playerStatsPanel = new PlayerStatsPanel(playState);
	playerStatsPanel->init();
	playerStatsPanel->setDirty(true);

	gameOverPanel = new GameOverPanel(playState);
	gameOverPanel->init();
}

void HudController::update(float elapsedTime) {
	if (currentScorePanel->isDirty()) {
		currentScorePanel->update(elapsedTime);
	}

	if (highScorePanel->isDirty()) {
		highScorePanel->update(elapsedTime);
	}

	if (playerStatsPanel->isDirty()) {
		playerStatsPanel->update(elapsedTime);
	}

	if (gameOverPanel->isVisible()) {
		gameOverPanel->update(elapsedTime);
	}
}

void HudController::draw() {
	currentScorePanel->draw();
	highScorePanel->draw();
	playerStatsPanel->draw();

	if (gameOverPanel->isVisible()) {
		gameOverPanel->draw();
	}
}
