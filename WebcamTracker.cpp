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


#include "WebcamTracker.h"

//WebcamTracker 
//the constructor, copy arguments, load the calibration file and init the camera
WebcamTracker::WebcamTracker( int w, int h,
							 int ix, int iy,int rball,
							 float vmax,
							 Renderer *renderer)
							 : Tracker(w, h, ix, iy, rball, vmax, renderer)
{
	cam = 0;

	bool colour = false;
	int fps=30, sourceDepth=8;
	this->cw = this->ch = 0;

	ifstream calfile;
	calfile.open("result.dat");
	if(calfile.fail()){
		cam = 0;
		return;
	}
	float tx, ty;
	calfile >> cx; calfile >> cy;
	calfile >> tx; calfile >> ty;
	calfile >> tx; calfile >> ty;
	calfile >> xx; calfile >> xy;
	calfile >> yx; calfile >> yy;
	calfile.close();

	camera = cameraTool::findCamera();	
	if (camera == NULL) {cam=0; return;}

	if (sourceDepth==24) colour = true;
	bool success = camera->initCamera(cw, ch, colour);
	
	if(success){

		this->cw = cw = camera->getWidth();
		this->ch = ch = camera->getHeight();
		fps = camera->getFps();

		printf("camera: %s\n",camera->getName());
		printf("format: %dx%d, %dfps\n\n",cw,ch,fps);
	} else {
		printf("could not initialize camera.\n");
		camera->closeCamera();
		delete camera;
		cam=0; camera = NULL;
		return;
	}

	success = camera->startCamera();
	if(!success){
		printf("cannot start camera.\n");
		camera->closeCamera();
		delete camera;
		cam=0; camera=NULL; return;
	}

	cam = 1; 
	camera->pauseCamera();
}

WebcamTracker::~WebcamTracker(void)
{
	if(cam){
		camera->closeCamera();
		delete camera;
	}
}

// GetHit 
// extract a hit: Track the motion of the club until it swings over
// the driving point
// The motion is tracked by searching the brightest pixel of the camera image
// and then taking the intensity-weihgted position average 
// in a square around this pixel
// 
void WebcamTracker::GetHit(float *v, float *phi){
	Uint32 clock = 0, clocklast=0;
	SDL_Event dummyevent;

	// do nothing if no camera is present
	if(cam==0)return; 

	unsigned char *image; // the frame captured from the webcam
	float pos[2], poslast[2]; // position of the club in the current and last frame
	int maxpos[2]; // position of the brightest pixel
	unsigned char max =0; // brightness of the brightest pixel
	float sum = 0; // brightness integral in the integrating square 
					//around the brightest pixel
	int mw=5; //halfwidth of the integrating square 
	int i,j;

	bool skipped=true; // flag indicating whether the last frame was skipped 
						// because the club was not visible

	camera->startCamera();

	while(SDL_PollEvent(&dummyevent)==0){ // SDL_PollEvent : emergency exit if the user presses any key
		sum=0;
		poslast[0] = pos[0];
		poslast[1] = pos[1];

		// acquire an image from the camera
		if((image = camera->getFrame())==NULL){continue;}
		clocklast = clock;
		clock = SDL_GetTicks();

		// find the brightest pixel
		max = 0;
		for(i=0; i<cw; i++){
			for(j=0; j<ch; j++){
				unsigned char pixel = image[j*cw+i];
				if(pixel >= max){
					max = pixel;
					maxpos[0] = i;
					maxpos[1] = j;
				}
			}
		}

		//skip the frame if it is clear that the brightest point cannot be the club
		if(max < 50) {skipped=true; continue;}
		if(sqrt((maxpos[0] - cx)*(maxpos[0] - cx) + 
			(maxpos[1] - cy)*(maxpos[1] - cy)) > 10*10)
		{skipped=true; continue;}

		// now compute the club position by taking the intnsity-weighted 
		// position average
		// of the area around the brightest pixel 
		pos[0] = pos[1] = 0;
		int xlower = maxpos[0]-mw; if(xlower < 0) xlower=0;
		int xupper = maxpos[0]+mw; if(xupper > cw) xupper=cw;
		int ylower = maxpos[1]-mw; if(ylower < 0) ylower=0;
		int yupper = maxpos[1]+mw; if(yupper > ch) yupper=ch;
		for(int i=xlower; i<xupper; i++){
			for(int j=ylower; j<yupper; j++){
				unsigned char pixel = image[j*cw+i];
				sum += (float)pixel;
				pos[0] += i*(float)pixel;
				pos[1] += j*(float)pixel;
			}
		}
		pos[0] = pos[0] / sum;
		pos[1] = pos[1] / sum;
		Uint32 clock_after_pos = SDL_GetTicks();

		//cout << pos[0] << " " << pos[1] << " " << (float)max << endl;

		// do not look for a hit if the last frame had been skipped
		if(skipped)
		{
			skipped=false; continue;
		}

		//			float target[2]; target[0] = cx; target[1] = cy;
		//			float lastnow[2]; lastnow[0]=pos[0]-poslast[0]; lastnow[1]=pos[1]-poslast[1]; 
		//			float tarlast[2]; tarlast[0]=target[0]-poslast[0];tarlast[1]=target[1]-poslast[1];
		//			float tarnow[2]; tarnow[0]=target[0]-pos[0];tarnow[1]=target[1]-pos[1];

		// create float values for screen coordinates of 
		//club position  in the current frame
		//club position  in the last frame
		//target position
		float fn[2]; fn[0]=pos[0];fn[1]=pos[1]; 
		float fl[2]; fl[0]=poslast[0];fl[1]=poslast[1]; 
		float ft[2]; ft[0]=cx;ft[1]=cy;
		//			float ft[2]; ft[0]=target[0];ft[1]=target[1];
		cam2scr(fn, fn+1);
		cam2scr(fl, fl+1);
		cam2scr(ft, ft+1);

		// compute difference vectors between
		// fln: last and current position
		// ftl: last position and target
		// ftn: cur. position and target
		float fln[2]; fln[0]=fn[0]-fl[0];fln[1]=fn[1]-fl[1];
		float ftl[2]; ftl[0]=fl[0]-ft[0];ftl[1]=fl[1]-ft[1];
		float ftn[2]; ftn[0]=fn[0]-ft[0];ftn[1]=fn[1]-ft[1];
		float nfln = sqrt(fln[0]*fln[0]+fln[1]*fln[1]);
		//			float flny[2]; 
		// compute projection of tl onto ln and its orthognal rest
		float nflnx = -(ftl[0]*fln[0]+ftl[1]*fln[1])/nfln;
		float nflny = sqrt(ftl[0]*ftl[0]+ftl[1]*ftl[1] - nflnx*nflnx);

		// finally, look for a hit
		if(ftl[0]*fln[0]+ftl[1]*fln[1] < 0 &&
			ftn[0]*fln[0]+ftn[1]*fln[1] > 0  && nflny < 50){
				camera->pauseCamera();
				*v = sqrt(fln[0]*fln[0]+ fln[1]*fln[1])/(clock-clocklast);
				*phi = atan(fln[1]/fln[0]);
				if(*v > vmax)
					*v=vmax;
				return;
		}
	}
}

// Dummy animator - if we are playing with a real club, we do not need 
// to animate the hit.
void WebcamTracker::AnimateHit(Uint32 duration, float v, float phi){
}

//cam2scr
// convert from camera coordinates to screen coordinates.
void WebcamTracker::cam2scr(float *x, float *y){
	float tx = *x-cx;
	float ty = *y-cy;
	*x = (float)ix + xx*tx + xy*ty;
	*y = (float)iy + yx*tx + yy*ty;
}

//cam2scr
// convert from camera coordinates to screen coordinates.
// not operational yet
void WebcamTracker::scr2cam(float *x, float *y){
	*x -= 320;
}