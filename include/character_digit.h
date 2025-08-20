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
#ifndef __CHARACTER_DIGIT_H
#define __CHARACTER_DIGIT_H

#define CHARACTER_DIGIT_WIDTH 4.0

enum SeparatorDigit {
	COMMA_SEPARATOR_DIGIT = 10
};

class CharacterDigit {
public:
	CharacterDigit(GameState* gameState);
	~CharacterDigit();

	bool isVisible() const {
		return visible;
	}

	int getActiveDigit() const {
		return activeDigit;
	}

	void setVisible(int activeDigit);

	void init(const NovaColor &color);
	void setSpriteColor(const NovaColor &color);
	void moveTo(float x, float y, float z);
	void transform();
	void draw();

	void reset() {
		this->activeDigit = 0;
		this->visible = false;
	}

protected:
	GameState* gameState;
	bool visible;
	std::vector<CharacterSprite*> characters;
	int activeDigit;
};

#endif // __CHARACTER_DIGIT_H
