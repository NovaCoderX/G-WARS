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

#ifndef __NOVA_COLOR_H
#define __NOVA_COLOR_H

enum ColorIndex {
	RED = 0, GREEN = 1, BLUE = 2
};

struct NovaColor {
	float data[3];

	NovaColor() {
		data[ColorIndex::RED] = 0;
		data[ColorIndex::GREEN] = 0;
		data[ColorIndex::BLUE] = 0;
	}

	NovaColor(int red, int green, int blue) {
		data[ColorIndex::RED] = (float) red / 255;
		data[ColorIndex::GREEN] = (float) green / 255;
		data[ColorIndex::BLUE] = (float) blue / 255;
	}

	int getRed() const {
		return (int) (data[ColorIndex::RED] * 255);
	}

	int getGreen() const {
		return (int) (data[ColorIndex::GREEN] * 255);
	}

	int getBlue() const {
		return (int) (data[ColorIndex::BLUE] * 255);
	}

	void setRed(int value) {
		data[ColorIndex::RED] = (float) value / 255;
	}

	void setGreen(int value) {
		data[ColorIndex::GREEN] = (float) value / 255;
	}

	void setBlue(int value) {
		data[ColorIndex::BLUE] = (float) value / 255;
	}

	void rebase(float percentage);
	bool fade(float amount, const NovaColor &targetColor);
	bool brighten(float amount, const NovaColor &targetColor);

	bool isBlack() const {
		if (data[ColorIndex::RED] == 0 && data[ColorIndex::GREEN] == 0 && data[ColorIndex::BLUE] == 0) {
			return true;
		}

		return false;
	}

	bool isWhite() const {
		if (data[ColorIndex::RED] == 1 && data[ColorIndex::GREEN] == 1 && data[ColorIndex::BLUE] == 1) {
			return true;
		}

		return false;
	}

	void reset() {
		data[ColorIndex::RED] = 0;
		data[ColorIndex::GREEN] = 0;
		data[ColorIndex::BLUE] = 0;
	}

	void operator +=(const NovaColor &a);
	void operator -=(const NovaColor &a);
	void operator *=(float val);
};

static inline bool operator ==(const NovaColor &a, const NovaColor &b) {
	if ((a.data[ColorIndex::RED] == b.data[ColorIndex::RED]) && (a.data[ColorIndex::GREEN] == b.data[ColorIndex::GREEN])
			&& (a.data[ColorIndex::BLUE] == b.data[ColorIndex::BLUE]))
		return true;
	else
		return false;
}

static inline bool operator !=(const NovaColor &a, const NovaColor &b) {
	if ((a.data[ColorIndex::RED] != b.data[ColorIndex::RED]) || (a.data[ColorIndex::GREEN] != b.data[ColorIndex::GREEN])
			|| (a.data[ColorIndex::BLUE] != b.data[ColorIndex::BLUE]))
		return true;
	else
		return false;
}

#endif // __NOVA_COLOR_H

