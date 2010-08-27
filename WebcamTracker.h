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


//WebcamTracker - configures a webcam, extracts the images and implements a simple 
// image recognition algorithm to find the position of the - infrared marked - club 
// in the scene. It tracks its position and detects the hits.
// To translate cam to screen coordinates, it needs to load a calibration file. 

#include "quantumminigolf.h"
#include "Tracker.h"
#include <SDL.h>
#include <math.h>
#include <cameraTool.h>
#include <iostream>
#include <fstream>

using namespace std;

class WebcamTracker : public Tracker
{
public:
	WebcamTracker(int w, int h, int ix, int iy, int rball,
				float vmax, Renderer *renderer);
	~WebcamTracker(void);

	void Init(){}
	void GetHit(float *v, float *phi);
	void AnimateHit(Uint32 duration, float v, float phi);

	int getix(){return ix;}
	int getiy(){return iy;}
	Renderer *renderer;
private:
	// conversion between camera and screen coordinates
	void cam2scr(float *x, float *y);
	void scr2cam(float *x, float *y); //scr2cam is not operational yet.
									  // provided only for future compatibility

	int rball;   // radius of the ball

	int cam;	// flag indicating whether a camera is present
	int cw, ch; // width and height of camera image;

	float xx, xy, yx, yy; // cam2screen matrix
	float cx, cy;		  // coordinates of the "Abschlag" in cam coordinates

	// handle to the webcam
	cameraEngine *camera;
};
