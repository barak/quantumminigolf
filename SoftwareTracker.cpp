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

#include "SoftwareTracker.h"

SoftwareTracker::SoftwareTracker(int w, int h, int ix, int iy, int rball, 
								 float vmax, Renderer *renderer) :
				Tracker( w, h, ix, iy, rball, vmax, renderer)
{
}

void SoftwareTracker::Init(void){

}

void SoftwareTracker::GetHit(float *v, float *phi){
		int mx, my;			// buffer variables for mouse position
		Uint32 sdlclock;
		SDL_Event dummyevent;

		while(SDL_PollEvent(&dummyevent)==0){
			// Adjust angle to mouse direction, fix it at mouse click
			SDL_GetMouseState(&mx, &my);
			*phi = M_PI*((float)my/h - .5);

			renderer->RenderTrack();
			renderer->RenderBall(ix, iy);
			renderer->RenderRacket(15, 20, ix, iy, *phi);
			renderer->Blit();
		}    
		// The user has clicked the mouse, wait until he releases it and determine 
		// the racket velocity from the hold time
		*v=0;
		sdlclock = SDL_GetTicks();
		while(SDL_PollEvent(&dummyevent)==0){
			*v = (float)(SDL_GetTicks() - sdlclock)/1000;
			if(*v > 1)
				*v = 1;

			renderer->RenderTrack();
			renderer->RenderBall(ix, iy);
			renderer->RenderRacket(15, 20+vmax * *v, ix, iy, *phi);
			renderer->Blit();
		}
}

// AnimateHit
// if software hit generation is enabled, draw a moving 
// club hitting the ball
void SoftwareTracker::AnimateHit(Uint32 duration, float v, float phi){
		Uint32 sdlclock = SDL_GetTicks();

		while(SDL_GetTicks() - sdlclock < duration){
			renderer->RenderTrack();
			renderer->RenderBall(ix, iy);
			renderer->RenderRacket(15, 
				20+vmax* v - (20-rball+vmax* v)*(float)(SDL_GetTicks() - sdlclock)*(SDL_GetTicks() - sdlclock)/duration/duration, 
				ix, iy, phi);
			renderer->Blit();
		}
}



SoftwareTracker::~SoftwareTracker(void)
{
}
