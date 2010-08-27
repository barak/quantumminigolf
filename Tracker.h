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


// Tracker -- an abstract base class defining the neccessary functionality 
// for a tracker class which can simulate a club and kick the quantum ball. 
// It is extended by SoftwareTracker and WebcamTracker for the software-only 
// and the virtual reality version of QMG respectively. 

#pragma once

#include "Renderer.h"
#include <SDL.h>
#include <math.h>

class Tracker
{
public:
	Tracker(int w, int h, int ix, int iy, int rball, float rvmax, Renderer *renderer);

	virtual void Init()=0;
	virtual void GetHit(float *v, float *phi)=0;
	virtual void AnimateHit(Uint32 duration, float v, float phi)=0;
	void setRenderer(Renderer *renderer){this->renderer = renderer;}

protected:
	~Tracker(void);
	int ix, iy, rball, w, h;
	float vmax;

	Renderer *renderer;
};
