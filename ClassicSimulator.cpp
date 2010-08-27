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


#include "ClassicSimulator.h"

ClassicSimulator::ClassicSimulator(int width, int height, Renderer *renderer, int holex, int holey, int holer)
{
	this->width = width;
	this->height = height;
	this->renderer = renderer;

	this->holex = holex;
	this->holey = holey;
	this->holer = holer;
}

ClassicSimulator::~ClassicSimulator(void)
{

}

void ClassicSimulator::setPosition(float x, float y){
	pos[0] = x; pos[1] = y;
}
void ClassicSimulator::setVelocity(float vx, float vy){
	vel[0] = vx, vel[1] = vy;
}
	

//propagate - propagation of the classical ball during dt milliseconds
// 
// it propagates the ball in mini-steps, each moving forward oonly one pixel
// according to the following scheme
//
//	1. move the ball one pixel ("trial move")
//  2. check for a wall collision (either with the hole or the 
//     "hard" part of the potential)
//	   In case of a collision, undo the trial move and change the velocity
//  3. Modify the velocity according to the soft potential
//  4. apply friction
//  5. goto 1
int ClassicSimulator::propagate(Uint32 dt){

	float speed = sqrt(vel[0]*vel[0] + vel[1]*vel[1]);

	// determine, how much 1-pixel will be neccessary during dt
	int steps = (int)(speed*dt); 
	float t, tic = 1/speed; // tic: time increment during a 1-pixel step

	// normalized components of the velocity
	float nvx = vel[0]/speed; 
	float nvy = vel[1]/speed;
	
	float slow = .001, shrink;

	Uint32 *sdat = (Uint32 *)(soft->pixels);
	Uint32 *hdat = (Uint32 *)(hard->pixels);
	
	bool hit = false; // have we hit a wall ?

	int x, y, xx, yy, xl, xu, yl, yu;
	float wx, wy, wn; // direction of the wall normal and wall vector length at a collision
	unsigned char pot = 0, mops;
	unsigned char lpot, rpot, upot, dpot; //potentials left /right / up /down the current position
										// used to compute potential gradient;
	bool inhole;

	// do steps iterations, each tic long, moving each time one pixel 
	// tic may change during the propagation due to a changing speed !
	for(t=0; t<dt; t+=tic){

		//check whether we are in the hole
		inhole = (holer*holer > 
			(pos[0]-holex)*(pos[0]-holex) + 
			(pos[1]-holey)*(pos[1]-holey) );

		// move the ball forward one pixel, check for collision
		pos[0] += nvx; pos[1] += nvy;
		x = (int)(pos[0]); y = (int)(pos[1]);
		if(pos[0] < 5) pos[0] = 5;
		if(pos[0] > width-5) pos[0] = width-5;
		if(pos[1] < 5) pos[1] = 5;
		if(pos[1] > height-5) pos[1] = height-5;

		// check whether we have left the hole, if so, simulate a wall 
		// collision to reflect from the hole wall
		if(inhole && (holer*holer < 
			(pos[0]-holex)*(pos[0]-holex) + 
			(pos[1]-holey)*(pos[1]-holey) )){
				hit = true;
				wx = pos[0]-holex; wy = pos[1]-holey;
				wn = sqrt(wx*wx+wy*wy);
				wx /= wn; wy /= wn;
		}
		// check whether we have collided with a potential wall
		SDL_GetRGB(hdat[y*width+x], hard->format, &pot, &mops, &mops);
		if(pot == 255 ){ // collision detected, determine the wall normal
						 // by taking the potential-weighted position average 
						 // in the 8x8 square around as
			hit = true;
			wx = 0; wy = 0;
			xl = x-3; if (xl < 0) xl=0;
			xu = x+4; if (xu > width) xu=width;
			yl = y-3; if (xl < 0) yl=0;
			yu = y+4; if (yu > height) yu=height;

			for(xx=xl; xx<xu; xx++){
				for(yy=yl; yy<yu; yy++){
					SDL_GetRGB(	hdat[yy*width+xx], hard->format, 
								&pot, &mops, &mops);
						wx += pot*(x-xx); wy += pot * (y-yy);
				}
			}
			wn = sqrt(wx*wx+wy*wy);
			wx /= wn;
			wy /= wn;
		}
		// we have been hit and know the wall normal
		// => undo test move to get off from the wall
		if(hit){
			pos[0] -= nvx; pos[1] -= nvy;
			x = (int)(pos[0]); y = (int)(pos[1]);

			float sp = vel[0]*wx + vel[1]*wy;
			float cvx = vel[0] - sp*wx; 
			float cvy = vel[1] - sp*wy; 
			vel[0] = -wx*sp +cvx; vel[1] = -wy*sp + cvy;

			nvx = vel[0]/speed;
			nvy = vel[1]/speed;
			hit=false;
		}

		//change velocity according to potential gradient
		if(y>0 && y<height-1 && x>0 && x<width-1){
			SDL_GetRGB(sdat[y*width+x-1], soft->format, &lpot, &mops, &mops);
			SDL_GetRGB(sdat[y*width+x+1], soft->format, &rpot, &mops, &mops);
			SDL_GetRGB(sdat[(y-1)*width+x], soft->format, &upot, &mops, &mops);
			SDL_GetRGB(sdat[(y+1)*width+x], soft->format, &dpot, &mops, &mops);
			vel[0] -= .002/1.2*tic*(rpot-lpot);
			vel[1] += .002/1.2*tic*(upot-dpot);
		}

		//simulate friction and update tic length due to changed speed
		speed = sqrt(vel[0]*vel[0] + vel[1]*vel[1]);
		shrink = 1 - tic*slow/speed; if(shrink<=0)shrink=0;
		speed *= shrink;
		vel[0]*= shrink; vel[1]*= shrink;
		if(speed==0)tic=1e4;
		else  tic = 1/speed;
	}
	
	if(speed ==0 )
		return 1;
	else
		return 0;
}

