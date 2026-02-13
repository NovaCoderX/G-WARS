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

#include <sys/time.h>

static timeval startTime;

DisplayManager::DisplayManager() {
	screen = NULL;
	numFrames = 0;
	fullscreen = false;
	mouseCaptured = false;
}

DisplayManager::~DisplayManager() {
	if (numFrames) {
		this->displayFrameRate();
	}

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void DisplayManager::init() {
	Yamlish* yamlish = g_worldManager->getYamlish();

	if (SDL_InitSubSystem(SDL_INIT_VIDEO) == -1) {
		fatalError("Could not initialize the video subsystem: %s\n", SDL_GetError());
	}

	// Note - Due to hardware platform limitations we only support a specific resolution and color depth.
	int width = yamlish->getInt("graphics.resolution.width", DISPLAY_WITDH);
	if (width != DISPLAY_WITDH) {
		fatalError("Screen width must be: %d\n", DISPLAY_WITDH);
	}

	int height = yamlish->getInt("graphics.resolution.height", DISPLAY_HEIGHT);
	if (height != DISPLAY_HEIGHT) {
		fatalError("Screen height must be: %d\n", DISPLAY_HEIGHT);
	}

	int depth = yamlish->getInt("graphics.resolution.depth", DISPLAY_DEPTH);
	if (depth != DISPLAY_DEPTH) {
		fatalError("Screen depth must be: %d\n", DISPLAY_DEPTH);
	}

	Uint32 flags = SDL_OPENGL;

	fullscreen = yamlish->getBool("graphics.fullscreen", true);
	if (fullscreen) {
		flags |= SDL_FULLSCREEN;
		logMessage("Trying to open a %dx%d %d-bit OpenGL screen....\n", width, height, depth);
	} else {
		logMessage("Trying to open a %dx%d %d-bit OpenGL window....\n", width, height, depth);
	}

	screen = SDL_SetVideoMode(width, height, depth, flags);
	if (screen == NULL) {
		if (fullscreen) {
			fatalError("Could not open a %dx%d %d-bit screen: %s\n", width, height, depth, SDL_GetError());
		} else {
			fatalError("Could not open a %dx%d %d-bit window: %s\n", width, height, depth, SDL_GetError());
		}
	}

	SDL_WM_SetCaption("G-WARS v3", NULL);

	// Disable OS cursor by default.
	grabMouse(true);

	// Setup OpenGL.
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, DISPLAY_WITDH, 0, DISPLAY_HEIGHT);

	// Store the start time.
	gettimeofday(&startTime, NULL);
}

void DisplayManager::grabMouse(bool enable) {
	if (enable) {
		if (SDL_WM_GrabInput(SDL_GRAB_QUERY) == SDL_GRAB_OFF) {
			SDL_WM_GrabInput(SDL_GRAB_ON);
		}
	} else {
		// Only release capture when in window mode.
		if (!fullscreen) {
			if (SDL_WM_GrabInput(SDL_GRAB_QUERY) == SDL_GRAB_ON) {
				SDL_WM_GrabInput(SDL_GRAB_OFF);
			}
		}
	}

	if (SDL_WM_GrabInput(SDL_GRAB_QUERY) == SDL_GRAB_ON) {
		// Only hide the mouse cursor when input is grabbed.
		SDL_ShowCursor(SDL_DISABLE);
		mouseCaptured = true;
	} else {
		SDL_ShowCursor(SDL_ENABLE);
		mouseCaptured = false;
	}
}

void DisplayManager::toggleMouseGrab() {
	if (!fullscreen) {
		this->grabMouse(!mouseCaptured);
	}
}

void DisplayManager::toggleFullScreen() {
	int width, height;
	Uint8 depth;
	Uint16 pitch;
	void* pixels;
	Uint32 flags;

	// Store in case we need to revert.
	width = screen->w;
	height = screen->h;
	depth = screen->format->BitsPerPixel;
	pitch = screen->pitch;
	flags = screen->flags;

	// Make a backup of the screen pixels before they get wiped.
	pixels = SDL_malloc(screen->h * screen->pitch);
	if (pixels) {
		SDL_memcpy(pixels, screen->pixels, (screen->h * screen->pitch));
	} else {
		logWarningMessage("Couldn't allocate a buffer to save the screen data\n");
	}

	// Toggle.
	fullscreen = (!fullscreen);

	if (fullscreen) {
		flags |= SDL_FULLSCREEN;
	} else {
		flags &= ~SDL_FULLSCREEN;
	}

	screen = SDL_SetVideoMode(width, height, depth, flags);
	if (!screen) {
		logWarningMessage("Could not toggle full screen mode, will try restoring the old mode\n");

		// Try swapping back.
		fullscreen = (!fullscreen);

		if (fullscreen) {
			flags |= SDL_FULLSCREEN;
		} else {
			flags &= ~SDL_FULLSCREEN;
		}

		screen = SDL_SetVideoMode(width, height, depth, flags);
		if (!screen) {
			fatalError("Could not restore the screen mode\n");
		}
	}

	// We need to redraw the screen.
	if (pixels) {
		if ((screen->h == height) && (screen->pitch == pitch)) {
			SDL_memcpy(screen->pixels, pixels, (screen->h * screen->pitch));
		}

		SDL_free(pixels);
		SDL_UpdateRect(screen, 0, 0, 0, 0);
	}


	if (fullscreen) {
		// Grab mouse if fullscreen.
		grabMouse(true);
	} else {
		// Release mouse if not fullscreen and the menu is active.
		if (g_worldManager->getActiveState() == MENU_STATE) {
			grabMouse(false);
		}
	}

	// Setup OpenGL (again).
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, DISPLAY_WITDH, 0, DISPLAY_HEIGHT);
}

void DisplayManager::displayFrameRate() {
	timeval endTime;
	double elapsedtime;

	gettimeofday(&endTime, NULL);

	// Compute and print the elapsed time in seconds.
	elapsedtime = (endTime.tv_sec - startTime.tv_sec) * 1000.0;      // sec to ms
	elapsedtime += (endTime.tv_usec - startTime.tv_usec) / 1000.0;   // us to ms

	logMessage("\n***************************************************\n");
	logMessage("GL_RENDERER = %s\n", (char*) glGetString(GL_RENDERER));
	logMessage("GL_VERSION = %s\n", (char*) glGetString(GL_VERSION));
	logMessage("GL_VENDOR = %s\n", (char*) glGetString(GL_VENDOR));

	float seconds = (elapsedtime / 1000.0);
	float fps = numFrames / seconds;
	logMessage("Rendered %d frames in %6.3f seconds (%6.3f FPS)\n", numFrames, seconds, fps);
	logMessage("***************************************************\n\n");
}

