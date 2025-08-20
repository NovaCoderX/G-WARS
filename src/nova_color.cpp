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

void NovaColor::rebase(float percentage) {
	if (percentage < 0) {
		logWarningMessage("Rebase percentage cannot be less than zero\n");
		percentage = 0;
	}

	// Percentage is used to adjust colors in a uniform manner.
	data[ColorIndex::RED] = (data[ColorIndex::RED] / 100) * percentage;
	data[ColorIndex::GREEN] = (data[ColorIndex::GREEN] / 100) * percentage;
	data[ColorIndex::BLUE] = (data[ColorIndex::BLUE] / 100) * percentage;

	// Must check for overflow.
	if (data[ColorIndex::RED] > 1)
		data[ColorIndex::RED] = 1;

	if (data[ColorIndex::GREEN] > 1)
		data[ColorIndex::GREEN] = 1;

	if (data[ColorIndex::BLUE] > 1)
		data[ColorIndex::BLUE] = 1;
}

bool NovaColor::fade(float amount, const NovaColor &targetColor) {
	int matchCount = 0;

	if (amount < 0) {
		logWarningMessage("Fade amount cannot be less than zero\n");
		amount = 0;
	}

	// Fade until we reach the target color.
	data[ColorIndex::RED] -= amount;
	data[ColorIndex::GREEN] -= amount;
	data[ColorIndex::BLUE] -= amount;

	// Must check for underflow.
	if (data[ColorIndex::RED] <= targetColor.data[ColorIndex::RED]) {
		data[ColorIndex::RED] = targetColor.data[ColorIndex::RED];
		matchCount++;
	}

	if (data[ColorIndex::GREEN] <= targetColor.data[ColorIndex::GREEN]) {
		data[ColorIndex::GREEN] = targetColor.data[ColorIndex::GREEN];
		matchCount++;
	}

	if (data[ColorIndex::BLUE] <= targetColor.data[ColorIndex::BLUE]) {
		data[ColorIndex::BLUE] = targetColor.data[ColorIndex::BLUE];
		matchCount++;
	}

	// Do all elements now match the target color?
	return (matchCount == 3);
}

bool NovaColor::brighten(float amount, const NovaColor &targetColor) {
	int matchCount = 0;

	if (amount < 0) {
		logWarningMessage("Brighten amount cannot be less than zero\n");
		amount = 0;
	}

	// Brighten until we reach the target color.
	data[ColorIndex::RED] += amount;
	data[ColorIndex::GREEN] += amount;
	data[ColorIndex::BLUE] += amount;

	// Must check for overflow.
	if (data[ColorIndex::RED] >= targetColor.data[ColorIndex::RED]) {
		data[ColorIndex::RED] = targetColor.data[ColorIndex::RED];
		matchCount++;
	}

	if (data[ColorIndex::GREEN] >= targetColor.data[ColorIndex::GREEN]) {
		data[ColorIndex::GREEN] = targetColor.data[ColorIndex::GREEN];
		matchCount++;
	}

	if (data[ColorIndex::BLUE] >= targetColor.data[ColorIndex::BLUE]) {
		data[ColorIndex::BLUE] = targetColor.data[ColorIndex::BLUE];
		matchCount++;
	}

	// Do all elements now match the target color?
	return (matchCount == 3);
}

void NovaColor::operator +=(const NovaColor &a) {
	data[ColorIndex::RED] += a.data[ColorIndex::RED];
	data[ColorIndex::GREEN] += a.data[ColorIndex::GREEN];
	data[ColorIndex::BLUE] += a.data[ColorIndex::BLUE];

	// Must check for overflow.
	if (data[ColorIndex::RED] > 1)
		data[ColorIndex::RED] = 1;

	if (data[ColorIndex::GREEN] > 1)
		data[ColorIndex::GREEN] = 1;

	if (data[ColorIndex::BLUE] > 1)
		data[ColorIndex::BLUE] = 1;
}

void NovaColor::operator -=(const NovaColor &a) {
	data[ColorIndex::RED] -= a.data[ColorIndex::RED];
	data[ColorIndex::GREEN] -= a.data[ColorIndex::GREEN];
	data[ColorIndex::BLUE] -= a.data[ColorIndex::BLUE];

	// Must check for underflow.
	if (data[ColorIndex::RED] < 0)
		data[ColorIndex::RED] = 0;

	if (data[ColorIndex::GREEN] < 0)
		data[ColorIndex::GREEN] = 0;

	if (data[ColorIndex::BLUE] < 0)
		data[ColorIndex::BLUE] = 0;
}

void NovaColor::operator *=(float amount) {
	data[ColorIndex::RED] *= amount;
	data[ColorIndex::GREEN] *= amount;
	data[ColorIndex::BLUE] *= amount;

	// Must check for overflow.
	if (data[ColorIndex::RED] > 1)
		data[ColorIndex::RED] = 1;

	if (data[ColorIndex::GREEN] > 1)
		data[ColorIndex::GREEN] = 1;

	if (data[ColorIndex::BLUE] > 1)
		data[ColorIndex::BLUE] = 1;

	// And underflow.
	if (data[ColorIndex::RED] < 0)
		data[ColorIndex::RED] = 0;

	if (data[ColorIndex::GREEN] < 0)
		data[ColorIndex::GREEN] = 0;

	if (data[ColorIndex::BLUE] < 0)
		data[ColorIndex::BLUE] = 0;
}

