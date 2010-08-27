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

#include "Tracker.h"

// SoftwareTracker - A class which implements a software-only golf club
// for all those who have not built the virtual reality version of the game
// It implements functions to hit the ball with the mouse

class SoftwareTracker :
	public Tracker
{
public:
	SoftwareTracker(int w, int h, int ix, int iy, int rball, 
		float vmax, Renderer *renderer);
	~SoftwareTracker(void);

	void Init();
	// Draw the racket and have the user choose the ball velocity by point-and-click
	void GetHit(float *v, float *phi);
	// Animate the racket kicking the ball
	void AnimateHit(Uint32 duration, float v, float phi);
};
