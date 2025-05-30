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
#ifndef __GAME_STATE_H
#define __GAME_STATE_H

// Base class for the different states of the game.
class GameState {
public:
	GameState() {}
	virtual ~GameState() {}

	virtual Camera* getCamera() const = 0;

	virtual void processInput() = 0;
	virtual void update() = 0;
	virtual void draw() = 0;
	virtual void reset() {} // Return to the initial state, can be used to start a new game...not the same as a refresh!

	// Functions called when a state is entered or left
	virtual void enterState() {} // Treated like a refresh, highscores can be updated etc
	virtual void leaveState() {} // Prob never used but maybe to stop playing music
};

#endif  // __GAME_STATE_H
