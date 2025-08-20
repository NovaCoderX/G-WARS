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
#ifndef __NOVA_UTILS_H
#define __NOVA_UTILS_H

extern void logMessage(const char *fmtString, ...);
extern void logWarningMessage(const char *fmtString, ...);
extern void logErrorMessage(const char *message);
extern void logShutdown(void);
extern void fatalError(const char *fmtString, ...);

static inline float float_rand(float minValue, float maxValue) {
	return minValue + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxValue-(minValue))));
}

static inline int int_rand(float minValue, float maxValue) {
	return int(round(minValue + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxValue-(minValue))))));
}

static inline double calculateDirectionFromVelocityComponents(double horizontalVelocity, double verticalVelocity) {
	// Convert from radians to degrees.
	double angle = atan2(verticalVelocity, horizontalVelocity) * (180.0 / M_PI);

	// Adjust angle to make up (positive y-axis) = 0°
	angle = (angle - 90);

	// Normalize to range [-180, 180]
	if (angle <= -180) {
		angle += 360;
	}

	return angle;
}

#endif // __NOVA_UTILS_H
