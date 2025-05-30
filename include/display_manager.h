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

#ifndef __DISPLAY_MANAGER_H
#define __DISPLAY_MANAGER_H

#define DISPLAY_WITDH 640
#define DISPLAY_HEIGHT 480
#define DISPLAY_DEPTH 16

class DisplayManager {
public:
	DisplayManager();
	~DisplayManager();

	void init();

	void begin() {
		// Clear the back buffer ready for the next frame.
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void end() {
		// Show the new frame.
		glFlush();
		SDL_GL_SwapBuffers();
		numFrames++;
	}

private:
	uint numFrames;

	void displayFrameRate();
};

#endif // __DISPLAY_MANAGER_H

