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
#ifndef __MENU_STATE_H
#define __MENU_STATE_H

enum MenuType {
	MAIN_MENU = 0,
	HIGHSCORE_MENU = 1,
	OPTIONS_MENU = 2
};

enum SpecialCharacter {
	DASH_CHARACTER_INDEX = 0,
	COLON_CHARACTER_INDEX = 1,
	CURSOR_CHARACTER_INDEX = 2
};

enum CharacterSize {
	LARGE_CHARACTER = 0,
	SMALL_CHARACTER = 1
};

class MenuState : public GameState
{
public:
	MenuState();
	~MenuState();

	Camera* getCamera() const {
		return camera;
	}

	SpriteDefinition* getAlphaCharacter(int index, CharacterSize size = SMALL_CHARACTER);
	SpriteDefinition* getSpecialCharacter(int index, CharacterSize size = SMALL_CHARACTER);
	SpriteDefinition* getNumericalCharacter(int index);
	void setActiveMenu(MenuType menu, bool reset = false);
	void init();
	void processInput();
	void update();
	void draw();
	void reset();
	void enterState();
	void leaveState();

private:
	Camera *camera;
	SpriteDefinition** smallAlphaCharacters;
	SpriteDefinition** largeAlphaCharacters;
	SpriteDefinition** smallSpecialCharactes;
	SpriteDefinition** largeSpecialCharactes;
	SpriteDefinition** numericalCharactes;
	BaseMenu* activeMenu;
	MainMenu* mainMenu;
	HighScoreMenu* highScoreMenu;
	OptionsMenu* optionsMenu;
	MenuBackground* menuBackground;
};

#endif  // __MENU_STATE_H
