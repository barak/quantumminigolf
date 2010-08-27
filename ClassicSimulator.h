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


//ClassicSimulator - A class to simulate a classic minigolf ball in the quantum minigolf playing 
// fields. It needs two bitmaps, one containing the infinitely high part of the potential, 
// folded with the ball shape and a second with the finitely hard part of the potential. 
//
// The classic simulator is still heavy work in progress

#include <SDL.h>
#include "quantumminigolf.h"
#include "Renderer.h"

class ClassicSimulator
{
public:
	ClassicSimulator(int width, int height, Renderer *renderer, int holex, int holey, int holer);

	void setPosition(float x, float y);
	void setVelocity(float vx, float vy);
	
	int propagate(Uint32 dt); // propagation function, takes timestep dt in SDL ticks and returns
							  // 1 if the ball stopped during the step, 0 otherwise
public:
	~ClassicSimulator(void);

	SDL_Surface *hard; // pointer to the inifinitely high part of the potential
	SDL_Surface *soft; // pointer to the finite height part of the potential

	float pos[2]; // position of the classical ball
	float vel[2]; // velocity of the classical ball

private:

	Renderer *renderer; // pointer to the renderer

	int width;
	int height;

	int holex, holey, holer;
};
