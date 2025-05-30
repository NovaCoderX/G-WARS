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

#ifdef GET_TIME_OF_DAY_FUNC
extern "C" {
	// Implementation using time() for systems without gettimeofday()
	int _gettimeofday(struct timeval *tv, void *tzvp) {
		time_t now = time(NULL);
		if (now == -1) {
		   return -1; // Indicate error
		}

		tv->tv_sec = now;
		tv->tv_usec = 0; // time() doesn't provide microseconds

		return 0;
   }
}
#endif

static SDL_Surface *screen = NULL;
static timeval startTime;

DisplayManager::DisplayManager() {
	numFrames = 0;
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

	logMessage("Trying to open a %dx%d %d-bit screen....\n", width, height, depth);

	// All screen res modes MUST be 4:3.
	screen = SDL_SetVideoMode(width, height, depth, SDL_OPENGL | SDL_FULLSCREEN);
	if (screen == NULL) {
		fatalError("Could not open a %dx%d %d-bit screen [%s]", width, height, depth, SDL_GetError());
	}

	// Disable OS cursor by default.
	SDL_WM_GrabInput(SDL_GRAB_ON);

	// Only hide the mouse cursor if we are grabbed
	if (SDL_WM_GrabInput(SDL_GRAB_QUERY) == SDL_GRAB_ON) {
		SDL_ShowCursor(SDL_DISABLE);
	} else {
		SDL_ShowCursor(SDL_ENABLE);
	}

	// Setup OpenGL.
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, DISPLAY_WITDH, 0, DISPLAY_HEIGHT);

	// Store the start time.
	gettimeofday(&startTime, NULL);
}

void DisplayManager::displayFrameRate() {
	timeval endTime;
	double elapsedtime;

	gettimeofday(&endTime, NULL);

    // Compute and print the elapsed time in seconds.
    elapsedtime = (endTime.tv_sec - startTime.tv_sec) * 1000.0;      // sec to ms
    elapsedtime += (endTime.tv_usec - startTime.tv_usec) / 1000.0;   // us to ms

    logMessage("\n***************************************************\n");
    logMessage("GL_RENDERER = %s\n", (char *) glGetString(GL_RENDERER));
    logMessage("GL_VERSION = %s\n", (char *) glGetString(GL_VERSION));
    logMessage("GL_VENDOR = %s\n", (char *) glGetString(GL_VENDOR));

    float seconds = (elapsedtime / 1000.0);
    float fps = numFrames / seconds;
    logMessage("Rendered %d frames in %6.3f seconds (%6.3f FPS)\n", numFrames, seconds, fps);
    logMessage("***************************************************\n\n");
}

