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

#ifndef __AUDIO_MANAGER_H
#define __AUDIO_MANAGER_H

#define DEFAULT_MIXER_CHANNELS 16
#define NUMBER_OF_SOUND_SAMPLES 32
#define DEFAULT_SOUND_VOLUME 5
#define MIN_SOUND_VOLUME 0
#define MAX_SOUND_VOLUME 10
#define DEFAULT_MUSIC_VOLUME 5
#define MIN_MUSIC_VOLUME 0
#define MAX_MUSIC_VOLUME 10

enum SampleType {
	SILENCE = 0,
	GAME_START = 1,
	GAME_OVER = 2,
	FIRE_NORMAL = 3,
	FIRE_HISPEED = 4,
	FIRE_HOMING = 5,
	FIRE_TRIWAY = 6,
	FIRE_SMARTBOMB = 7,
	FIRE_SMARTBOMB_LOW = 8,
	ENEMY_SPAWN_ROCKET_SHIP = 9, // Enemy_spawn_green
	ENEMY_SPAWN_FLYING_SAUCER = 10, // Enemy_spawn_orange
	ENEMY_SPAWN_PLAYER_CLONE = 11, // Enemy_spawn_purple
	ENEMY_SPAWN_BLACK_HOLE = 12, // Enemy_spawn_red
	ENEMY_SPAWN_MINI_GATE = 13, // Enemy_spawn_blue
	ENEMY_SPAWN_SNAKE = 14,
	ENEMY_EXPLODE = 15,
	PLAYER_SPAWN = 16,
	PLAYER_SHIELD_ON = 17,
	PLAYER_SHIELD_OFF = 18,
	PLAYER_THRUST_LOOP = 19,
	PLAYER_HIT_WALL = 20, // Ship_hitwall
	PLAYER_SHIELD_HIT_ENEMY = 21,
	PLAYER_EXPLODE = 22,
	PICKUP_MULTIPLIER = 23, // Nugget
	PICKUP_POWER_UP = 24, // pickup_weapon
	AWARDED_EXTRA_LIFE = 25,
	AWARDED_SMARTBOMB = 26,
	MISSILE_EXPLODE = 27, // bullet_hitwall
	LASER_EXPLODE = 28, // bullet_hitwall
	HIGH_SCORE_ACHIEVED = 29,
	UI_CHANGE = 30,
	UI_CONFIRM = (NUMBER_OF_SOUND_SAMPLES - 1)
};

enum MusicType {
    NO_MUSIC = 0,
	STANDARD_MUSIC = 1,
	BOSS_MUSIC = 2
};

class AudioManager {
public:
	AudioManager();
	~AudioManager();

	void init();
	void startSound(SampleType sampleType);
	void startMusic(bool fadeIn = true);
	void stopMusic(bool fadeOut = true);
	void pauseMusic();
	void resumeMusic();
	bool isMusicPaused();

	void setMusicType(MusicType musicType) {
		this->musicType = musicType;
	}

private:
	bool soundEnabled;
	bool musicEnabled;
	MusicType musicType;
};

#endif // __AUDIO_MANAGER_H

