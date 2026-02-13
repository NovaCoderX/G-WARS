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

#define DEFAULT_MIXER_CHANNELS 32
#define NUMBER_OF_SOUND_SAMPLES 35
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
	ENEMY_SPAWN_BLACK_HOLE = 12, // Repulsar_spawn
	ENEMY_SPAWN_MINI_GATE = 13, // Enemy_spawn_blue
	ENEMY_SPAWN_MINI_CRUSHER = 14, // Enemy_spawn_red
	ENEMY_SPAWN_BOSS = 15, // Snake_spawn
	STANDARD_ALIEN_EXPLODE = 16, // Enemy_explode
	SPECIAL_ALIEN_EXPLODE = 17, //  Gravity_well_die
	BOSS_ALIEN_EXPLODE = 18, //  Gravity_well_explode
	PLAYER_SPAWN = 19,
	PLAYER_SHIELD_ON = 20,
	PLAYER_SHIELD_OFF = 21,
	PLAYER_THRUST_LOOP = 22,
	PLAYER_HIT_WALL = 23, // Ship_hitwall
	PLAYER_SHIELD_HIT_ENEMY = 24,
	PLAYER_EXPLODE = 25, // Ship_explode
	PICKUP_MULTIPLIER = 26, // Multiplier
	PICKUP_POWER_UP = 27, // pickup_weapon
	PICKUP_EXTRA_LIFE = 28, // pickup_extralife
	PICKUP_SMARTBOMB = 29, // pickup_smartbomb
	MISSILE_EXPLODE = 30, // bullet_hitwall
	LASER_EXPLODE = 31, // bullet_hitwall
	HIGH_SCORE_ACHIEVED = 32,
	UI_CHANGE = 33,
	UI_CONFIRM = (NUMBER_OF_SOUND_SAMPLES - 1)
};

enum MusicType {
    NO_MUSIC = 0,
	GAMEPLAY_MUSIC = 1,
	BOSS_MUSIC = 2,
	MENU_MUSIC = 3
};

class AudioManager {
public:
	AudioManager();
	~AudioManager();

	void init();
	int startSound(SampleType sampleType);
	bool isSoundPlaying(int channel);
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

