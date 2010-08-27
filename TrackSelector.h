#pragma once
/*	Quantum Minigolf, a computer game illustrating quantum mechanics
	Copyright (C) 2007 Friedemann Reinhard <friedemann.reinhard@gmail.com>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


// TrackSelector -- a class to preload several tracks and have the user choose one of them
// It is responsible for the keyboard handling while the user navigates through the menu

#include "Renderer.h"
#include "ClassicSimulator.h"
#include <SDL.h>
#include <iostream>
#include <vector>
#include <list>
#include <string>

using namespace std;

typedef struct{
	SDL_Surface *V;
	SDL_Surface *hard;
	SDL_Surface *soft;
}trackrecord;

class TrackSelector
{
public:
	TrackSelector(Renderer *renderer, ClassicSimulator *csimulator);
	~TrackSelector(void);

	// handle the user input in the menu phase (i.e. cycle the 
	// tracks and then quit or start a new game)
	int GetTrack(bool *quantum);

private:
	SDL_Surface *BlackTrack(void);

	Renderer *renderer;  // pointer to the renderer
	ClassicSimulator *csimulator; // pointer to the simulator - 
								 // used to reset csimulator.hard
								 // and csimulator.soft
	int width; int height; 

	bool help;  // display help menu

	// tracks - a list containing all the pre-loaded tracks 
	// when the user cycles through the tracks, they are fetched 
	// from 'tracks' by 'trackiterator'
	list<trackrecord *> tracks;
	list<trackrecord *>::iterator trackiterator;
};
