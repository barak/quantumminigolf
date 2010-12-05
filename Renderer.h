#pragma once

//Renderer - a class encapsulating all neccessary graphics functions,
// such as drawing balls, holes, menus, waves and blitting the rendered
// graphics to the screen.
// There is one global Renderer instance, to which many other classes posess
// pointers
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


#include <math.h>
#include <stdlib.h>
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include <fftw3.h>

using namespace std;

#include "quantumminigolf.h"

class Renderer
{
public:
  Renderer (int width, int height, int flag, int holex, int holey, int holer,
	    int rball);
   ~Renderer (void);

  void RenderTrack (void);
  void RenderBlack (void);
  void RenderHole (float x, float y, float r);
  void RenderWave (fftwf_complex * psi);
  void RenderBall (float posx, float posy);
  void RenderRacket (float l, float r, int ix, int iy, float rphi);
  void RenderFlash (void);	// show a white surface
  void RenderExtro (int result, int ypos);	// show the "You win" / "You lose"
  void RenderMenu (bool quantum);

  void Lock (void);
  void Unlock (void);
  void Blit (void);

  void SaveFrame (const char *fname);

  void ToggleCmap ()
  {
    cmap = ((cmap == cmapm) ? cmapc : cmapm);
  }

  void ToggleBackgroundRendering ()
  {
	renderTrack = !renderTrack;
	renderHole = !renderHole;
  }

  SDL_Surface *screen, *V;
  int width;
  int height;

private:
  SDL_Surface * bBuffer;	// the back buffer
  SDL_Surface *wave;		// the wave
  SDL_Surface *cmapm, *cmapc, *cmap;	// monochrome, colored and a pointer to the actual color map
  SDL_Surface *win;
  SDL_Surface *lose;
  SDL_Rect rBuffer;

  // fonts for the menu
  TTF_Font *fntsml;
  TTF_Font *fntbg;

  int holex, holey, holer;	// position and radius of the hole
  int rball;			// radius of the ball

  bool renderTrack, renderHole;
};
