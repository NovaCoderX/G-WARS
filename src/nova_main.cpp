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

#include <time.h>

// Global world pointer.
WorldManager *g_worldManager = NULL;

static void nova_exit_handler() {
	if (g_worldManager) {
		// Deleting the World cleans up everything else.
		delete g_worldManager;
		g_worldManager = NULL;
	}

	// Apart from the logger.
	logShutdown();
}

extern "C" {
	void poly_nova_main(char *baseDirectory) {
		atexit(nova_exit_handler);

		// Seed the  pseudo-random number generator.
		srand(time(NULL));

		// Create our world object (global pointer), then run it.
		g_worldManager = new WorldManager();
		g_worldManager->load(baseDirectory);
		g_worldManager->run();
	}
}

