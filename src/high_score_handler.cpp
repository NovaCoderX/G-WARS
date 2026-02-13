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

/* Third-party JSON library */
#include "cJSON.h"

extern "C" {
	int NetworkConnect();
	void NetworkDisconnect();
	char* PerformHTTPSRequest(const char* method, const char* path, const char* body);
}

// TODO - move to compiler defs
#define API_TOKEN_READ "ktnpbrrbtfmzr"
#define API_TOKEN_UPDATE "hsvxlwrdqfvyp"
#define API_TOKEN_ADMIN "jvwcfvzcxdmre"

#define MAX_SCORE_VALUE 9999999
#define MIN_ONLINE_SCORE_VALUE 100000
#define MAX_SCORE_ENTRIES 10

// A simple key for XORing. Keep this secret in your source code!
const char SECRET_KEY[] = "CopperList1987";

struct PlayerTokenRecord {
    uint32_t id;
} __attribute__((packed));

struct PlayerDataRecord {
    char name[MAX_PLAYER_NAME_LENGTH];
    uint32_t score;
} __attribute__((packed));

struct HighScoreRecord {
    uint32_t id;
    char name[MAX_PLAYER_NAME_LENGTH];
    uint32_t score;
} __attribute__((packed));


static inline bool is_blank(const std::string& s) {
    // Returns true if length is 0 OR all characters are whitespace
    return std::all_of(s.begin(), s.end(), [](unsigned char ch) {
        return std::isspace(ch);
    });
}

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

static inline void toUpper(std::string& s) {
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
		return static_cast<char>(std::toupper(c));
	});
}

static std::string formatStringWithCommas(int value) {
	char buffer[20];

	std::snprintf(buffer, sizeof(buffer), "%d", value);
	std::string numStr(buffer);
	int insertPosition = numStr.length() - 3;

	while (insertPosition > 0) {
		numStr.insert(insertPosition, ",");
		insertPosition -= 3;
	}

	return numStr;
}

static std::string getPlayerName() {
	std::string playerName = g_worldManager->getYamlish()->get("player.name");
	if (is_blank(playerName)) {
		playerName = DEFAULT_PLAYER_NAME;
	}

	trim(playerName);
	truncate(playerName, MAX_PLAYER_NAME_LENGTH);
	toUpper(playerName);

	return playerName;
}

HighScoreHandler::HighScoreHandler() {
	online = false;
	playerToken = 0;
}

HighScoreHandler::~HighScoreHandler() {
	highScores.clear();
}

void HighScoreHandler::init() {
	// Read player token and data.
	this->loadPlayerToken("token.dat");
	this->loadLocalPlayerData("local_player.dat");

	if (playerToken) {
		this->loadRegisteredPlayerData("registered_player.dat");
	}

#ifndef TIME_DEMO_ENABLED
	// See if the player wants to use the online features.
	Yamlish* yamlish = g_worldManager->getYamlish();
	online = yamlish->getBool("highscores.online", false);
#endif

	if (online) {
		if (NetworkConnect() != 0) {
			logWarningMessage("Cannot connect to the network, check your network settings\n");
		} else {
			// Connected, try to refresh high scores from the server.
			this->loadOnlineScores();
			NetworkDisconnect();
		}
	}

	if (highScores.empty()) {
		// Either we are offline or the online refresh failed, use the local data instead.
		this->loadLocalScores("scores.dat");
	}

	bool found = false;
	for (size_t i = 0; i < highScores.size(); i++) {
		if (highScores[i].id == playerToken) {
			// Sync the high score data.
			highScores[i].name = localPlayerData.name;
			highScores[i].score = localPlayerData.score;
			found = true;
			break;
		}
	}

	if (!found) {
		// Player is not in the high score data so we need to add them.
		highScores.push_back(HighScoreData(playerToken, localPlayerData.name, localPlayerData.score));
	}

	if (!highScores.empty()) {
		// Sort the table
		std::sort(highScores.begin(), highScores.end());

		// Clamp entries.
		while (highScores.size() > MAX_SCORE_ENTRIES) {
			highScores.pop_back();
		}
	}

	// Sync the local data in case the player has changed the Yaml file between games.
	this->syncOptions();
}


uint HighScoreHandler::getHighestScore() {
	uint highestScore = 0;

	if (!highScores.empty()) {
		highestScore = highScores[0].score;
	}

	return highestScore;
}

void HighScoreHandler::syncOptions() {
	// See if the player wants to use the online features.
	Yamlish* yamlish = g_worldManager->getYamlish();
	online = yamlish->getBool("highscores.online", false);

	// Sync the local data in case the player has just changed their name.
	localPlayerData.name = getPlayerName();

	// We also need to sync the high score data in case the player has just changed their name.
	if (!highScores.empty()) {
		for (size_t i = 0; i < highScores.size(); i++) {
			if (highScores[i].id == playerToken) {
				highScores[i].name = localPlayerData.name;
				break;
			}
		}
	}
}

void HighScoreHandler::setPlayerScore(uint score) {
	if (score > MAX_SCORE_VALUE) {
		score = MAX_SCORE_VALUE;
	}

	if (score > localPlayerData.score) {
		localPlayerData.score = score;
	}

	// Update the high score data.
	bool found = false;
	for (size_t i = 0; i < highScores.size(); i++) {
		if (highScores[i].id == playerToken) {
			highScores[i].score = localPlayerData.score;
			found = true;
			break;
		}
	}

	if (!found) {
		// Player is not in the high score data so we need to add them.
		highScores.push_back(HighScoreData(playerToken, localPlayerData.name, localPlayerData.score));

		// Sort the table
		std::sort(highScores.begin(), highScores.end());

		// Clamp entries.
		while (highScores.size() > MAX_SCORE_ENTRIES) {
			highScores.pop_back();
		}
	}
}

void HighScoreHandler::save() {
	if (online) {
		bool valid = true;

		if (localPlayerData.score < MIN_ONLINE_SCORE_VALUE) {
			std::string formattedNumber = formatStringWithCommas(MIN_ONLINE_SCORE_VALUE);
			logWarningMessage("Player high score must be greater than %s points to enable online saving\n", formattedNumber.c_str());
			valid = false;
		}

		if (localPlayerData.name == DEFAULT_PLAYER_NAME) {
			logWarningMessage("Cannot save Player high score online without a valid player name\n");
			valid = false;
		}

		if (valid) {
			if (NetworkConnect() != 0) {
				logWarningMessage("Cannot connect to the network, check your network settings\n");
			} else {
				// Connected.
				if (!playerToken) {
					// Player is not currently registered with the scoreboard.
					this->registerPlayer();

					// Update the high score data with the new token to keep in sync.
					if (playerToken) {
						for (size_t i = 0; i < highScores.size(); i++) {
							if (highScores[i].id == 0) {
								highScores[i].id = playerToken;
								break;
							}
						}
					}
				} else {
					// Update a registered player.
					this->syncRegisteredPlayer();
				}

				NetworkDisconnect();
			}
		}
	}

	this->savePlayerToken("token.dat");
	this->saveLocalPlayerData("local_player.dat");

	if (playerToken) {
		this->saveRegisteredPlayerData("registered_player.dat");
	}

	this->saveLocalScores("scores.dat");
}

void HighScoreHandler::loadPlayerToken(const std::string& filename) {
	bool loaded = false;

	FILE* file = fopen(filename.c_str(), "rb");
	if (file) {
		PlayerTokenRecord data;

		if (fread(&data, sizeof(data), 1, file) == 1) {
			// Decrypt the struct (XORing again restores the original data)
			char* dataPtr = reinterpret_cast<char*>(&data);
			for (size_t i = 0; i < sizeof(data); ++i) {
				dataPtr[i] ^= SECRET_KEY[i % sizeof(SECRET_KEY)];
			}

			playerToken = data.id;
			loaded = true;
		}

		fclose(file);

		// See if was successful.
		if (!loaded) {
			logWarningMessage("Cannot load the player token file, a new token will be created\n");

			// Set the default.
			playerToken = 0;
		} else {
			logMessage("Loaded the player token file\n");
		}
	}
}

void HighScoreHandler::loadLocalPlayerData(const std::string& filename) {
	bool loaded = false;

	FILE* file = fopen(filename.c_str(), "rb");
	if (file) {
		PlayerDataRecord data;

		if (fread(&data, sizeof(data), 1, file) == 1) {
			// Decrypt the struct (XORing again restores the original data)
			char* dataPtr = reinterpret_cast<char*>(&data);
			for (size_t i = 0; i < sizeof(data); ++i) {
				dataPtr[i] ^= SECRET_KEY[i % sizeof(SECRET_KEY)];
			}

			localPlayerData.name = data.name;
			localPlayerData.score = data.score;
			loaded = true;
		}

		fclose(file);
	}

	// See if was successful.
	if (!loaded) {
		logWarningMessage("Cannot load the local player data file, a new file will be created\n");

		// Set the defaults.
		localPlayerData.name = getPlayerName();
		localPlayerData.score = 0;
	} else {
		logMessage("Loaded the local player data file\n");
	}
}

void HighScoreHandler::loadRegisteredPlayerData(const std::string& filename) {
	bool loaded = false;

	FILE* file = fopen(filename.c_str(), "rb");
	if (file) {
		PlayerDataRecord data;

		if (fread(&data, sizeof(data), 1, file) == 1) {
			// Decrypt the struct (XORing again restores the original data)
			char* dataPtr = reinterpret_cast<char*>(&data);
			for (size_t i = 0; i < sizeof(data); ++i) {
				dataPtr[i] ^= SECRET_KEY[i % sizeof(SECRET_KEY)];
			}

			registeredPlayerData.name = data.name;
			registeredPlayerData.score = data.score;
			loaded = true;
		}

		fclose(file);
	}

	// See if was successful.
	if (!loaded) {
		logWarningMessage("Cannot load the registered player data file, a new token will be created\n");

		// Reset (player is no longer registered).
		playerToken = 0;
	} else {
		logMessage("Loaded the registered player data file\n");
	}
}

void HighScoreHandler::loadLocalScores(const std::string& filename) {
	logMessage("Trying to load the local high score data...\n");

	// Clear any existing data.
    highScores.clear();

    FILE* file = fopen(filename.c_str(), "rb");
    if (!file) {
        logWarningMessage("Cannot open the local high score file: %s\n", filename.c_str());
        return;
    }

    HighScoreRecord record;

    // Read one record at a time until EOF
    while (fread(&record, sizeof(record), 1, file) == 1) {
        // XOR Decrypt
        char* ptr = reinterpret_cast<char*>(&record);
        for (size_t i = 0; i < sizeof(record); ++i) {
            ptr[i] ^= SECRET_KEY[i % sizeof(SECRET_KEY)];
        }

        // Process the decrypted data.
        highScores.push_back(HighScoreData(record.id, record.name, record.score));
    }

    fclose(file);

    if (highScores.empty()) {
        logWarningMessage("Could not load the local high score data\n");
    } else {
        logMessage("Loaded %d high scores\n", (int)highScores.size());
    }
}

void HighScoreHandler::loadOnlineScores() {
	logMessage("Trying to load the online high score data...\n");

	// Clear any existing data.
	highScores.clear();

	char get_path[128];
	snprintf(get_path, sizeof(get_path), "/api/%s/board/", API_TOKEN_READ);

	char* response = PerformHTTPSRequest("GET", get_path, NULL);
	if (response) {
		// Parse the response.
		cJSON* root = cJSON_Parse(response);
		if (root) {
			cJSON* players = cJSON_GetObjectItem(root, "players");
			if (cJSON_IsArray(players)) {
				int num_players = cJSON_GetArraySize(players);

				for (int i = 0; i < num_players; i++) {
					cJSON* p = cJSON_GetArrayItem(players, i);
					cJSON* id_obj = cJSON_GetObjectItem(p, "id");
					cJSON* name_obj = cJSON_GetObjectItem(p, "name");
					cJSON* score_obj = cJSON_GetObjectItem(p, "score");
					highScores.push_back(HighScoreData(id_obj->valueint, name_obj->valuestring, score_obj->valueint));
				}
			}

			cJSON_Delete(root);
		}

		free(response);
	}

	// See if was successful.
	if (highScores.empty()) {
		logWarningMessage("Could not load the online high score data\n");
	} else {
		logMessage("Loaded %d high scores\n", (int)highScores.size());
	}
}

void HighScoreHandler::registerPlayer() {
	logMessage("Trying to register player with the online scoreboard...\n");

	// Build the JSON body using cJSON
	cJSON *body_root = cJSON_CreateObject();
	if (body_root) {
		cJSON_AddStringToObject(body_root, "name", localPlayerData.name.c_str());
		cJSON_AddBoolToObject(body_root, "is_eliminated", false);
		cJSON_AddBoolToObject(body_root, "is_visible", true);
		cJSON_AddNullToObject(body_root, "text_color");
		cJSON_AddNullToObject(body_root, "background_color");
		cJSON_AddNullToObject(body_root, "profile_image");
		cJSON_AddNullToObject(body_root, "team");
		cJSON_AddNullToObject(body_root, "data");
		cJSON_AddNumberToObject(body_root, "score", localPlayerData.score);
		cJSON_AddNullToObject(body_root, "scores");
		cJSON_AddNumberToObject(body_root, "goal", 0);

		char *body_string = cJSON_PrintUnformatted(body_root);
		if (body_string) {
			char post_path[128];
			snprintf(post_path, sizeof(post_path), "/api/%s/player/", API_TOKEN_ADMIN);
			char* response = PerformHTTPSRequest("POST", post_path, body_string);
			if (response) {
				cJSON *response_root = cJSON_Parse(response);
				if (response_root) {
					// Get the "player" nested object
					cJSON *player = cJSON_GetObjectItemCaseSensitive(response_root, "player");

					// Get the "id" item from the player object
					if (cJSON_IsObject(player)) {
						cJSON *id_item = cJSON_GetObjectItemCaseSensitive(player, "id");
						if (cJSON_IsNumber(id_item)) {
							playerToken = id_item->valueint;
						}
					}

					cJSON_Delete(response_root);
				}
			}

			free(body_string);
		}

		cJSON_Delete(body_root);
	}

	// See if was successful.
	if (!playerToken) {
		logWarningMessage("Could not register player: %s\n", localPlayerData.name.c_str());
	} else {
		logMessage("Registered player: %s\n", localPlayerData.name.c_str());
		registeredPlayerData.name = localPlayerData.name;
		registeredPlayerData.score = localPlayerData.score;
	}
}

void HighScoreHandler::syncRegisteredPlayer() {
	logMessage("Trying to synchronise a registered player with the online scoreboard...\n");
	const char* successString = "\"success\":true}";

	if (localPlayerData.name != registeredPlayerData.name) {
		cJSON* body_root = cJSON_CreateObject();
		if (body_root) {
			cJSON_AddStringToObject(body_root, "name", localPlayerData.name.c_str());

			char *body_string = cJSON_PrintUnformatted(body_root);
			if (body_string) {
				char post_path[128];
				snprintf(post_path, sizeof(post_path), "/api/%s/player/%u", API_TOKEN_UPDATE, playerToken);

				char* response = PerformHTTPSRequest("PATCH", post_path, body_string);
				if (response) {
					if (strstr(response, successString) != nullptr) {
						logMessage("Player name synchronised with the server\n");
						registeredPlayerData.name = localPlayerData.name;
					}
				}

				free(body_string);
			}

			cJSON_Delete(body_root);
		}
	}

	if (localPlayerData.score > registeredPlayerData.score) {
		cJSON* body_root = cJSON_CreateObject();
		if (body_root) {
			cJSON_AddNumberToObject(body_root, "player_id", playerToken);
			cJSON_AddNumberToObject(body_root, "score", localPlayerData.score);
			cJSON_AddNumberToObject(body_root, "goal", 0);
			cJSON_AddStringToObject(body_root, "operation", "set");

			char *body_string = cJSON_PrintUnformatted(body_root);
			if (body_string) {
				char post_path[128];
				snprintf(post_path, sizeof(post_path), "/api/%s/score/", API_TOKEN_UPDATE);
				char* response = PerformHTTPSRequest("POST", post_path, body_string);
				if (response) {
					if (strstr(response, successString) != nullptr) {
						logMessage("Player score synchronised with the server\n");
						registeredPlayerData.score = localPlayerData.score;
					}
				}

				free(body_string);
			}

			cJSON_Delete(body_root);
		}
	}
}

void HighScoreHandler::savePlayerToken(const std::string& filename) {
	PlayerTokenRecord data;
	data.id = playerToken;

	// Encrypt the struct in memory before saving.
	char* dataPtr = reinterpret_cast<char*>(&data);
	for (size_t i = 0; i < sizeof(data); ++i) {
		dataPtr[i] ^= SECRET_KEY[i % sizeof(SECRET_KEY)];
	}

	FILE* file = fopen(filename.c_str(), "wb");
	if (!file) {
		logWarningMessage("Cannot save the player token\n");
	}

	if (file) {
		fwrite(&data, sizeof(data), 1, file);
		fclose(file);
	}
}

void HighScoreHandler::saveLocalPlayerData(const std::string& filename) {
	PlayerDataRecord data;

	strncpy(data.name, localPlayerData.name.c_str(), sizeof(data.name) - 1);
	data.name[sizeof(data.name) - 1] = '\0';
	data.score = localPlayerData.score;

	// Encrypt the struct in memory before saving.
	char* dataPtr = reinterpret_cast<char*>(&data);
	for (size_t i = 0; i < sizeof(data); ++i) {
		dataPtr[i] ^= SECRET_KEY[i % sizeof(SECRET_KEY)];
	}

	FILE* file = fopen(filename.c_str(), "wb");
	if (!file) {
		logWarningMessage("Cannot save the local player data\n");
	}

	if (file) {
		fwrite(&data, sizeof(data), 1, file);
		fclose(file);
	}
}

void HighScoreHandler::saveRegisteredPlayerData(const std::string& filename) {
	PlayerDataRecord data;

	strncpy(data.name, registeredPlayerData.name.c_str(), sizeof(data.name) - 1);
	data.name[sizeof(data.name) - 1] = '\0';
	data.score = registeredPlayerData.score;

	// Encrypt the struct in memory before saving.
	char* dataPtr = reinterpret_cast<char*>(&data);
	for (size_t i = 0; i < sizeof(data); ++i) {
		dataPtr[i] ^= SECRET_KEY[i % sizeof(SECRET_KEY)];
	}

	FILE* file = fopen(filename.c_str(), "wb");
	if (!file) {
		logWarningMessage("Cannot save the registered player data\n");
	}

	if (file) {
		fwrite(&data, sizeof(data), 1, file);
		fclose(file);
	}
}

void HighScoreHandler::saveLocalScores(const std::string& filename) {
    if (!highScores.empty()) {
		FILE* file = fopen(filename.c_str(), "wb");
		if (!file) {
			logWarningMessage("Cannot save the local high score data\n");
		}

		if (file) {
			for (const auto& entry : highScores) {
				HighScoreRecord record;
				record.id = entry.id;
				record.score = entry.score;

				// Copy name and ensure it is null-terminated
				strncpy(record.name, entry.name.c_str(), sizeof(record.name) - 1);
				record.name[sizeof(record.name) - 1] = '\0';

				// XOR Encrypt the record
				char* ptr = reinterpret_cast<char*>(&record);
				for (size_t i = 0; i < sizeof(record); ++i) {
					ptr[i] ^= SECRET_KEY[i % sizeof(SECRET_KEY)];
				}

				fwrite(&record, sizeof(record), 1, file);
			}

			fclose(file);
		}
    }
}

