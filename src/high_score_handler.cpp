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

#include <algorithm>

#define MAX_PLAYER_NAME_LENGTH 12
#define MAX_SCORE_VALUE 9999999
#define MAX_ENTRIES 8

static inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
	}));
}

static inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
	}).base(), s.end());
}

static inline void trim(std::string& s) {
	ltrim(s);
	rtrim(s);
}

static inline void truncate(std::string& s, std::size_t length) {
	if (s.size() > length) {
		s.resize(length);
	}
}

HighScoreHandler::HighScoreHandler() {
	// Empty.
}

HighScoreHandler::~HighScoreHandler() {
	highScores.clear();
}

bool HighScoreHandler::load(const std::string& filename) {
	FILE* file = fopen(filename.c_str(), "r");
	if (!file) {
		return false;
	}

	char buffer[80];
	char name[20];
	int score;

	while (fgets(buffer, sizeof(buffer), file)) {
		if (sscanf(buffer, "%19[^,],%d", name, &score) == 2) {
			std::string validatedName = name;
			if (validatedName.empty()) {
				validatedName = DEFAULT_PLAYER_NAME;
			}

			trim(validatedName);
			truncate(validatedName, MAX_PLAYER_NAME_LENGTH);

			if (score < 0) {
				score = 0;
			}

			if (score > MAX_SCORE_VALUE) {
				score = MAX_SCORE_VALUE;
			}

			highScores.push_back(HighScoreData(validatedName, score));
		} else {
			break;
		}
	}

	fclose(file);

	// Sort the table
	std::sort(highScores.begin(), highScores.end());

	// Clamp entries.
	while (highScores.size() > MAX_ENTRIES) {
		highScores.pop_back();
	}

	return true;
}

bool HighScoreHandler::save(const std::string& filename) {
	FILE *file = fopen(filename.c_str(), "w");
	if (!file) {
		return false;
	}

	for (HighScoreData highScore : highScores) {
		fprintf(file, "%s,%u\n", highScore.name.c_str(), highScore.score);
	}

	fclose(file);
	return true;
}

uint HighScoreHandler::getHighScore() {
	uint highestScore = 0;

	if (highScores.size() > 0) {
		highestScore = highScores[0].score;
	}

	return highestScore;
}

void HighScoreHandler::addScore(uint score) {
	if (score > MAX_SCORE_VALUE) {
		score = MAX_SCORE_VALUE;
	}

	std::string name = g_worldManager->getYamlish()->get("player.name");
	if (name.empty()) {
		name = DEFAULT_PLAYER_NAME;
	}
	trim(name);
	truncate(name, MAX_PLAYER_NAME_LENGTH);


	highScores.push_back(HighScoreData(name, score));

	// Sort the table
	std::sort(highScores.begin(), highScores.end());

	// Clamp entries.
	while (highScores.size() > MAX_ENTRIES) {
		highScores.pop_back();
	}
}
