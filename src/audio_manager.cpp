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

#include <SDL/SDL_mixer.h>

#define MIX_FADE_TIME 3000
#define NUMBER_OF_MUSIC_TRACKS 4

enum MusicTrack {
	EVOLVED = 0,
	WAVES = 1,
	DEADLINE = 2,
	KING = 3
};

static Mix_Chunk *samples[NUMBER_OF_SOUND_SAMPLES] = { NULL };
static Mix_Music *music[NUMBER_OF_MUSIC_TRACKS] = { NULL };

AudioManager::AudioManager() {
	musicEnabled = soundEnabled = false;
	musicType = STANDARD_MUSIC;
}

AudioManager::~AudioManager() {
	if (musicEnabled || soundEnabled) {
		// Free the samples.
		for (int i = 0; i < NUMBER_OF_SOUND_SAMPLES; i++) {
			Mix_FreeChunk(samples[i]);
			samples[i] = NULL;
		}

		// Free the music.
		for (int i = 0; i < NUMBER_OF_MUSIC_TRACKS; i++) {
			Mix_FreeMusic(music[i]);
			music[i] = NULL;
		}

		Mix_CloseAudio();
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
	}
}

void AudioManager::init() {
	int mixer_freq;
	Uint16 mixer_format;
	int mixer_channels;
	char filepath[PATH_MAX];

	int soundVolumeIndex = g_worldManager->getYamlish()->getInt("audio.sound.volume", DEFAULT_SOUND_VOLUME );
	if (soundVolumeIndex < MIN_SOUND_VOLUME ) {
		soundVolumeIndex = MIN_SOUND_VOLUME ;
	}

	if (soundVolumeIndex > MAX_SOUND_VOLUME ) {
		soundVolumeIndex = MAX_SOUND_VOLUME ;
	}

	if (soundVolumeIndex) {
		logMessage("Sound enabled\n");
		soundEnabled = true;
	}

	int musicVolumeIndex = g_worldManager->getYamlish()->getInt("audio.music.volume", DEFAULT_MUSIC_VOLUME);
	if (musicVolumeIndex < MIN_MUSIC_VOLUME) {
		musicVolumeIndex = MIN_MUSIC_VOLUME;
	}

	if (musicVolumeIndex > MAX_MUSIC_VOLUME) {
		musicVolumeIndex = MAX_MUSIC_VOLUME;
	}

	if (musicVolumeIndex) {
		logMessage("Music enabled\n");
		musicEnabled = true;
	}

	if (soundEnabled || musicEnabled) {
		if (SDL_InitSubSystem(SDL_INIT_AUDIO) == -1) {
			fatalError("Could not initialize the audio subsystem: %s\n", SDL_GetError());
		}

		int frequency = g_worldManager->getYamlish()->getInt("audio.mixer.quality", MIX_DEFAULT_FREQUENCY);
		if (Mix_OpenAudio(frequency, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) {
			fatalError("Could not initialize the audio mixer: %s\n", Mix_GetError());
		}

		if (!Mix_QuerySpec(&mixer_freq, &mixer_format, &mixer_channels)) {
			fatalError("Could not initialize the audio mixer: %s\n", Mix_GetError());
		}

		if (mixer_channels != 2) {
			logWarningMessage("Could not allocate stereo audio\n");
		} else {
			logMessage("Using 16-bit stereo audio frequency: %d\n", mixer_freq);
		}

		int mixerChannels = g_worldManager->getYamlish()->getInt("audio.mixer.channels", DEFAULT_MIXER_CHANNELS);
		logMessage("Using %d mixing channels\n", Mix_AllocateChannels(mixerChannels));
	}

	if (soundEnabled) {
		// Pre-load all of the sound samples used by the game.
		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "silence");
		samples[SILENCE] = Mix_LoadWAV(filepath);
		if (!samples[SILENCE]) {
			logWarningMessage("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Game_start");
		samples[GAME_START] = Mix_LoadWAV(filepath);
		if (!samples[GAME_START]) {
			logWarningMessage("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Game_over");
		samples[GAME_OVER] = Mix_LoadWAV(filepath);
		if (!samples[GAME_OVER]) {
			logWarningMessage("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Fire_normal");
		samples[FIRE_NORMAL] = Mix_LoadWAV(filepath);
		if (!samples[FIRE_NORMAL]) {
			logWarningMessage("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Fire_Hispeed");
		samples[FIRE_HISPEED] = Mix_LoadWAV(filepath);
		if (!samples[FIRE_HISPEED]) {
			fatalError("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Fire_homing");
		samples[FIRE_HOMING] = Mix_LoadWAV(filepath);
		if (!samples[FIRE_HOMING]) {
			fatalError("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Fire_triway");
		samples[FIRE_TRIWAY] = Mix_LoadWAV(filepath);
		if (!samples[FIRE_TRIWAY]) {
			fatalError("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Fire_smartbomb");
		samples[FIRE_SMARTBOMB] = Mix_LoadWAV(filepath);
		if (!samples[FIRE_SMARTBOMB]) {
			logWarningMessage("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Fire_smartbomb_low");
		samples[FIRE_SMARTBOMB_LOW] = Mix_LoadWAV(filepath);
		if (!samples[FIRE_SMARTBOMB_LOW]) {
			fatalError("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Enemy_spawn_green");
		samples[ENEMY_SPAWN_ROCKET_SHIP] = Mix_LoadWAV(filepath);
		if (!samples[ENEMY_SPAWN_ROCKET_SHIP]) {
			fatalError("Could not load sound sample: %s\n", filepath);
		}
		
		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Enemy_spawn_orange");
		samples[ENEMY_SPAWN_FLYING_SAUCER] = Mix_LoadWAV(filepath);
		if (!samples[ENEMY_SPAWN_FLYING_SAUCER]) {
			fatalError("Could not load sound sample: %s\n", filepath);
		}
		
		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Enemy_spawn_purple");
		samples[ENEMY_SPAWN_PLAYER_CLONE] = Mix_LoadWAV(filepath);
		if (!samples[ENEMY_SPAWN_PLAYER_CLONE]) {
			fatalError("Could not load sound sample: %s\n", filepath);
		}
		
		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Enemy_spawn_red");
		samples[ENEMY_SPAWN_BLACK_HOLE] = Mix_LoadWAV(filepath);
		if (!samples[ENEMY_SPAWN_BLACK_HOLE]) {
			fatalError("Could not load sound sample: %s\n", filepath);
		}
		
		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Enemy_spawn_blue");
		samples[ENEMY_SPAWN_MINI_GATE] = Mix_LoadWAV(filepath);
		if (!samples[ENEMY_SPAWN_MINI_GATE]) {
			fatalError("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Snake_spawn");
		samples[ENEMY_SPAWN_SNAKE] = Mix_LoadWAV(filepath);
		if (!samples[ENEMY_SPAWN_SNAKE]) {
			fatalError("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Enemy_explode");
		samples[ENEMY_EXPLODE] = Mix_LoadWAV(filepath);
		if (!samples[ENEMY_EXPLODE]) {
			logWarningMessage("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Player_Spawn");
		samples[PLAYER_SPAWN] = Mix_LoadWAV(filepath);
		if (!samples[PLAYER_SPAWN]) {
			logWarningMessage("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Shield_on");
		samples[PLAYER_SHIELD_ON] = Mix_LoadWAV(filepath);
		if (!samples[PLAYER_SHIELD_ON]) {
			logWarningMessage("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Shield_off");
		samples[PLAYER_SHIELD_OFF] = Mix_LoadWAV(filepath);
		if (!samples[PLAYER_SHIELD_OFF]) {
			logWarningMessage("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Ship_thrust_loop");
		samples[PLAYER_THRUST_LOOP] = Mix_LoadWAV(filepath);
		if (!samples[PLAYER_THRUST_LOOP]) {
			logWarningMessage("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Ship_hitwall");
		samples[PLAYER_HIT_WALL] = Mix_LoadWAV(filepath);
		if (!samples[PLAYER_HIT_WALL]) {
			logWarningMessage("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Player_shielded_hit_enemy");
		samples[PLAYER_SHIELD_HIT_ENEMY] = Mix_LoadWAV(filepath);
		if (!samples[PLAYER_SHIELD_HIT_ENEMY]) {
			logWarningMessage("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Ship_explode");
		samples[PLAYER_EXPLODE] = Mix_LoadWAV(filepath);
		if (!samples[PLAYER_EXPLODE]) {
			logWarningMessage("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Multiplier");
		samples[PICKUP_MULTIPLIER] = Mix_LoadWAV(filepath);
		if (!samples[PICKUP_MULTIPLIER]) {
			logWarningMessage("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "pickup_weapon");
		samples[PICKUP_POWER_UP] = Mix_LoadWAV(filepath);
		if (!samples[PICKUP_POWER_UP]) {
			logWarningMessage("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "pickup_extralife");
		samples[AWARDED_EXTRA_LIFE] = Mix_LoadWAV(filepath);
		if (!samples[AWARDED_EXTRA_LIFE]) {
			logWarningMessage("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "pickup_smartbomb");
		samples[AWARDED_SMARTBOMB] = Mix_LoadWAV(filepath);
		if (!samples[AWARDED_SMARTBOMB]) {
			logWarningMessage("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "bullet_hitwall");
		samples[MISSILE_EXPLODE] = Mix_LoadWAV(filepath);
		if (!samples[MISSILE_EXPLODE]) {
			logWarningMessage("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "bullet_hitwall");
		samples[LASER_EXPLODE] = Mix_LoadWAV(filepath);
		if (!samples[LASER_EXPLODE]) {
			logWarningMessage("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "Hi_Score_achieved");
		samples[HIGH_SCORE_ACHIEVED] = Mix_LoadWAV(filepath);
		if (!samples[HIGH_SCORE_ACHIEVED]) {
			logWarningMessage("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "UI_Change");
		samples[UI_CHANGE] = Mix_LoadWAV(filepath);
		if (!samples[UI_CHANGE]) {
			logWarningMessage("Could not load sound sample: %s\n", filepath);
		}

		sprintf(filepath, "%s/Sounds/%s.wav", g_worldManager->getBaseDirectory().c_str(), "UI_Confirm");
		samples[UI_CONFIRM] = Mix_LoadWAV(filepath);
		if (!samples[UI_CONFIRM]) {
			logWarningMessage("Could not load sound sample: %s\n", filepath);
		}

		int volume = (MIX_MAX_VOLUME * ((float)soundVolumeIndex / 10));
		logMessage("Sound FX volume is set to: %d\n", volume);

		for (int i = 0; i < NUMBER_OF_SOUND_SAMPLES; i++) {
			Mix_VolumeChunk(samples[i], volume);
		}
	}

	if (musicEnabled) {
		// Pre-load all of the music used by the game.
		sprintf(filepath, "%s/Music/%s.ogg", g_worldManager->getBaseDirectory().c_str(), "Evolved");
		music[EVOLVED] = Mix_LoadMUS(filepath);
		if (!music[EVOLVED]) {
			logWarningMessage("Could not load music track: %s\n", filepath);
		}

		sprintf(filepath, "%s/Music/%s.ogg", g_worldManager->getBaseDirectory().c_str(), "Waves");
		music[WAVES] = Mix_LoadMUS(filepath);
		if (!music[WAVES]) {
			logWarningMessage("Could not load music track: %s\n", filepath);
		}

		sprintf(filepath, "%s/Music/%s.ogg", g_worldManager->getBaseDirectory().c_str(), "Deadline");
		music[DEADLINE] = Mix_LoadMUS(filepath);
		if (!music[DEADLINE]) {
			logWarningMessage("Could not load music track: %s\n", filepath);
		}

		sprintf(filepath, "%s/Music/%s.ogg", g_worldManager->getBaseDirectory().c_str(), "King");
		music[KING] = Mix_LoadMUS(filepath);
		if (!music[KING]) {
			logWarningMessage("Could not load music track: %s\n", filepath);
		}

		int volume = (MIX_MAX_VOLUME * ((float)musicVolumeIndex / 10));
		logMessage("Music volume is set to: %d\n", volume);

		for (int i = 0; i < NUMBER_OF_SOUND_SAMPLES; i++) {
			Mix_VolumeMusic(volume);
		}
	}

	if (musicEnabled || soundEnabled) {
		SDL_PauseAudio(0);
	}
}

void AudioManager::startSound(SampleType sampleType) {
	if (soundEnabled) {
		if (samples[sampleType]) {
			// Pick the first free channel.
			Mix_PlayChannel(-1, samples[sampleType], 0);
		}
	}
}

void AudioManager::startMusic(bool fadeIn) {
	static int track = -1;

	if (musicEnabled) {
		// Stop any currently playing music.
		stopMusic(false);

        if (musicType != NO_MUSIC) {
    		if (musicType == STANDARD_MUSIC) {
    			track++;
    			if (track > DEADLINE) {
    				track = EVOLVED;
    			}
    		} else {
    			// Boss music.
    			track = KING;
    		}
    
    		if (music[track]) {
    			if (fadeIn) {
    				Mix_FadeInMusic(music[track], -1, MIX_FADE_TIME);
    			} else {
    				Mix_PlayMusic(music[track], -1);
    			}
    		}
        }
	}
}

void AudioManager::stopMusic(bool fadeOut) {
	if (musicEnabled) {
		if (Mix_PlayingMusic()) {
			if (fadeOut) {
				Mix_FadeOutMusic(MIX_FADE_TIME);
			} else {
				Mix_HaltMusic();
			}
		}
	}
}

void AudioManager::pauseMusic() {
	if (musicEnabled) {
		if (Mix_PlayingMusic()) {
			Mix_PauseMusic();
		}
	}
}

void AudioManager::resumeMusic() {
	if (musicEnabled) {
		if (this->isMusicPaused()) {
			Mix_ResumeMusic();
		} else {
			// Restart.
			this->startMusic();
		}
	}
}

bool AudioManager::isMusicPaused() {
	bool paused = false;

	if (musicEnabled) {
		if (Mix_PausedMusic()) {
			paused = true;
		}
	}

	return paused;
}



