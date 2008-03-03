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

#include "Renderer.h"

// Constructor:
// initialize all graphics buffers, load the complex color map and the fonts
Renderer::Renderer (int width, int height, int flag, int holex, int holey,
		    int holer, int rball)
{
  this->width = width;
  this->height = height;

  this->holex = holex;
  this->holey = holey;
  this->holer = holer;
  this->rball = rball;

#ifdef _PROFILE
  char dummy[80];
  SDL_VideoInfo *scrnfo;
#endif

  SDL_Init (SDL_INIT_VIDEO);
  SDL_WM_SetCaption ("Quantum Minigolf", NULL);
  SDL_WM_SetIcon (SDL_LoadBMP ("gfx/icon.bmp"), NULL);

#ifdef _PROFILE
  scrnfo = SDL_GetVideoInfo ();
  printf ("Video hardware info:\n");
  printf ("hw_available: %d\n", scrnfo->hw_available);
  printf ("wm_available: %d\n", scrnfo->hw_available);
  printf ("blit_sw: %d\n", scrnfo->blit_sw);
  printf ("blit_sw_CC: %d\n", scrnfo->blit_sw_CC);
  printf ("video_mem: %d\n", scrnfo->video_mem);
  printf ("optimal color depth: %d bits\n", scrnfo->vfmt->BitsPerPixel);
  SDL_VideoDriverName (dummy, sizeof (dummy));
  printf ("video driver: %s\n", dummy);
  printf ("\n");
#endif

#ifdef VR
  screen = SDL_SetVideoMode (640, 480, 32, flag | SDL_HWACCEL);
#endif
#ifndef VR
  screen = SDL_SetVideoMode (width, height, 32, flag | SDL_HWACCEL);
#endif

  bBuffer = SDL_CreateRGBSurface (SDL_SWSURFACE, screen->w,
				  screen->h,
				  screen->format->BitsPerPixel,
				  screen->format->Rmask,
				  screen->format->Gmask,
				  screen->format->Bmask,
				  screen->format->Amask);

  if (!bBuffer)
    {
      printf ("Error: Can't create SDL_SWSURFACE\n\n");
      exit (1);
    }

  wave = SDL_CreateRGBSurface (SDL_SWSURFACE | SDL_SRCALPHA, screen->w,
			       screen->h,
			       32, 0xff << 24, 0xff << 16, 0xff << 8, 0xff);
  if (!wave)
    {
      printf ("Error: Can't create SDL_SWSURFACE\n\n");
      exit (1);
    }

  cmapm = SDL_LoadBMP ("gfx/cmap_mono.bmp");
  cmapc = SDL_LoadBMP ("gfx/cmap.bmp");
  win = SDL_LoadBMP ("gfx/win.bmp");
  lose = SDL_LoadBMP ("gfx/lose.bmp");

  if (cmapm == NULL)
    {
      printf ("Error: Can't load bitmap cmap_mono\n\n");
      exit (1);
    }
  if (cmapc == NULL)
    {
      printf ("Error: Can't load bitmap cmap\n\n");
      exit (1);
    }
  if (win == NULL)
    {
      printf ("Error: Can't load bitmap win\n\n");
      exit (1);
    }
  if (lose == NULL)
    {
      printf ("Error: Can't load bitmap lose\n\n");
      exit (1);
    }

  // compute transparency data of the color maps
  cmapc =
    SDL_ConvertSurface (cmapc, wave->format, SDL_SWSURFACE | SDL_SRCALPHA);
  Uint32 *pdata = (Uint32 *) cmapc->pixels;
  for (int i = 0; i < cmapc->w; i++)
    {
      for (int j = 0; j < cmapc->h; j++)
	{
	  unsigned char red, green, blue, alpha;
	  SDL_GetRGBA (pdata[j * cmapc->w + i], cmapc->format, &red, &green,
		       &blue, &alpha);
	  alpha = red;
	  if (green > alpha)
	    alpha = green;
	  if (blue > alpha)
	    alpha = blue;
	  if (alpha > 0)
	    {
	      red = min (red * 255 / alpha, 255);
	      green = min (green * 255 / alpha, 255);
	      blue = min (blue * 255 / alpha, 255);
	    }
	  pdata[j * cmapc->w + i] =
	    SDL_MapRGBA (cmapc->format, red, green, blue, alpha);
	}
    }

  cmapm =
    SDL_ConvertSurface (cmapm, wave->format, SDL_SWSURFACE | SDL_SRCALPHA);
  pdata = (Uint32 *) cmapm->pixels;
  for (int i = 0; i < cmapm->w; i++)
    {
      for (int j = 0; j < cmapm->h; j++)
	{
	  unsigned char red, green, blue, alpha;
	  SDL_GetRGBA (pdata[j * cmapm->w + i], cmapm->format, &red, &green,
		       &blue, &alpha);
	  alpha = red;
	  if (green > alpha)
	    alpha = green;
	  if (blue > alpha)
	    alpha = blue;
	  if (alpha > 0)
	    {
	      red = min (red * 255 / alpha, 255);
	      green = min (green * 255 / alpha, 255);
	      blue = min (blue * 255 / alpha, 255);
	    }
	  pdata[j * cmapm->w + i] =
	    SDL_MapRGBA (cmapm->format, red, green, blue, alpha);
	}
    }
  cmap = cmapc;

  win = SDL_ConvertSurface (win, screen->format, SDL_SWSURFACE);
  lose = SDL_ConvertSurface (lose, screen->format, SDL_SWSURFACE);

  SDL_SetColorKey (win, SDL_SRCCOLORKEY,
		   SDL_MapRGB (win->format, 255, 255, 255));
  SDL_SetColorKey (lose, SDL_SRCCOLORKEY,
		   SDL_MapRGB (lose->format, 255, 255, 255));

  //load the fonts for the menu - preferably tahoma, otherwise LinLiberty
  TTF_Init ();

#ifdef LINUX
  fntsml = TTF_OpenFont ("fonts/default.ttf", 12);
  fntbg = TTF_OpenFont ("fonts/default.ttf", 24);
#endif

#ifdef WIN32
  fntsml = TTF_OpenFont ("C:\\windows\\fonts\\tahoma.ttf", 12);
  if (fntsml == NULL)
    {
      fntsml = TTF_OpenFont ("fonts/default.ttf", 12);
    }

  fntbg = TTF_OpenFont ("C:\\windows\\fonts\\tahoma.ttf", 24);
  if (fntbg == NULL)
    {
      fntbg = TTF_OpenFont ("fonts/default.ttf", 24);
    }
#endif

  rBuffer.x = 0;
#ifdef VR
  rBuffer.y = 160;		//160
#endif
#ifndef VR
  rBuffer.y = 0;
#endif

  rBuffer.w = bBuffer->w;
  rBuffer.h = bBuffer->h;

  SDL_EventState (SDL_ACTIVEEVENT, SDL_IGNORE);
  SDL_EventState (SDL_MOUSEMOTION, SDL_IGNORE);
  SDL_EventState (SDL_KEYUP, SDL_IGNORE);
  SDL_EventState (SDL_KEYDOWN, SDL_ENABLE);

  SDL_ShowCursor (SDL_DISABLE);
}


Renderer::~Renderer (void)
{
  TTF_CloseFont (fntsml);
  TTF_CloseFont (fntbg);
  TTF_Quit ();

  SDL_FreeSurface (bBuffer);
  SDL_FreeSurface (wave);
  SDL_FreeSurface (cmap);
  SDL_FreeSurface (win);
  SDL_FreeSurface (lose);
  SDL_Quit ();
}


// RenderTrack
// render the bare track (i.e.: the potential bitmap) without any balls, clubs, and holes
void
Renderer::RenderTrack ()
{
  SDL_Rect rblit;

  rblit.x = 0;
  rblit.y = 0;
  rblit.w = bBuffer->w;
  rblit.h = bBuffer->h;

  SDL_BlitSurface (V, NULL, bBuffer, &rblit);
}

//RenderHole
//render the (minimalistic, OK) hole into the bBuffer
void
Renderer::RenderHole (float x, float y, float r)
{
  float phi;

  Uint32 *buffer_dat = (Uint32 *) bBuffer->pixels;
  SDL_PixelFormat *fmt = bBuffer->format;

  for (phi = 0; phi < 2 * M_PI; phi += .01)
    {
      buffer_dat[(int) (x + r * sin (phi)) +
		 (int) (y + r * cos (phi)) * width] =
	SDL_MapRGB (fmt, 255, 255, 0);
    }
}



//RenderWave
//given the complex wavefunction psi render it into buffer
//nota bene: The color coding of the wavefunction is performed by mapping it
//to the precalculated color map 'cmap'
void
Renderer::RenderWave (fftwf_complex * psi)
{
  Uint32 *buffer_dat, *cmap_dat, *V_dat, *wave_dat;
  int x, y;
  int cw = cmap->w;
  int ch = cmap->h;
  int cx, cy;
  Uint8 red, dummy;

  SDL_PixelFormat *fmt = bBuffer->format;

  buffer_dat = (Uint32 *) bBuffer->pixels;
  cmap_dat = (Uint32 *) cmap->pixels;
  V_dat = (Uint32 *) V->pixels;
  wave_dat = (Uint32 *) wave->pixels;

  for (x = 0; x < width; x++)
    {
      for (y = 0; y < height; y++)
	{
	  cx = (int) (psi[x * height + y][1]) + 128;
	  cy = (int) (psi[x * height + y][0]) + 128;
	  if (cx > 240)
	    cx = 240;
	  if (cx < 0)
	    cx = 0;
	  if (cy > 240)
	    cy = 240;
	  if (cy < 0)
	    cy = 0;
	  wave_dat[y * width + x] = cmap_dat[cx * cw + cy];
	}
    }
  SDL_Rect rblit;

  rblit.x = 0;
  rblit.y = 0;
  rblit.w = bBuffer->w;
  rblit.h = bBuffer->h;

  SDL_BlitSurface (wave, NULL, bBuffer, &rblit);

  RenderHole (holex, holey, holer);
}

// RenderBall
// render the (classical) ball at a position posx, posy into buffer.
// used at the beginning (to draw the ball at the "Abschlag")
// and at the end (i.e. after the position measurement)
void
Renderer::RenderBall (float posx, float posy)
{
  SDL_PixelFormat *fmt = bBuffer->format;

  float r, phi;
  Uint32 *buffer_dat = (Uint32 *) bBuffer->pixels;

  // render the ball
  for (phi = 0; phi < 2 * M_PI; phi += .1)
    {
      for (r = 0; r < rball; r += .5)
	{
	  buffer_dat[(int) (posx + r * sin (phi)) +
		     (int) (posy + r * cos (phi)) * width] =
	    SDL_MapRGB (fmt, 0, 0, 255);
	}
    }

  RenderHole (holex, holey, holer);
}

// Render the racket
// l: length of the racket
// r: distance from ball
// ix, iy: position of the ball
// mousey: mouse y position
void
Renderer::RenderRacket (float l, float r, int ix, int iy, float rphi)
{
  float xl, xo, yl, yo;		// upper and lower coordinate bounds of the racket
  float dx, dy;			// x and y increments
  int x, y;

  float i;

  SDL_PixelFormat *fmt = bBuffer->format;

  Uint32 *buffer_dat = (Uint32 *) bBuffer->pixels;

  xo = ix + r * cos (rphi) - .5 * l * sin (rphi);
  xl = ix + r * cos (rphi) + .5 * l * sin (rphi);
  yo = iy + r * sin (rphi) + .5 * l * cos (rphi);
  yl = iy + r * sin (rphi) - .5 * l * cos (rphi);

  dx = xo - xl;
  dy = yo - yl;

  for (i = 0; i < 1; i += .01)
    {
      x = (int) (xl + i * dx);
      if (x > (width - 1))
	x = width - 1;
      if (x < 0)
	x = 0;
      y = (int) (yl + i * dy);
      if (y > (height - 1))
	y = height - 1;
      if (y < 0)
	y = 0;
      buffer_dat[y * width + x] = SDL_MapRGB (fmt, 255, 255, 0);
    }
}

// RenderFlash
// Render a flash (one white frame) to indicate the position measurement
void
Renderer::RenderFlash ()
{
  int x, y;

  SDL_PixelFormat *fmt = bBuffer->format;

  Uint32 *buffer_dat = (Uint32 *) bBuffer->pixels;

  for (x = 0; x < width; x++)
    {
      for (y = 0; y < height; y++)
	{
	  buffer_dat[y * width + x] = SDL_MapRGB (fmt, 255, 255, 255);
	}
    }
}

// RenderExtro
// Depending on result, render either the win / lose message
// at position ypos.
void
Renderer::RenderExtro (int result, int ypos)
{
  SDL_Rect rResblit;
  SDL_Surface *res;

  if (result == QMG_WIN)
    res = win;
  else
    res = lose;

  if (ypos > height / 2 - res->h)
    ypos = height / 2 - res->h;

  rResblit.x = width / 2 - res->w / 2;
  rResblit.y = ypos;
  rResblit.w = res->w;
  rResblit.h = res->h;

  SDL_BlitSurface (res, NULL, bBuffer, &rResblit);
}

// RenderMenu
// render an info screen with
// transparent background which will be overlayed when selecting the track
void
Renderer::RenderMenu (bool quantum)
{
  int linesep = 20;

  if (fntbg == NULL || fntsml == NULL)
    return;

  SDL_Color clrFg = { 255, 0, 0, 255 };	// Red ("Fg" is foreground)
  SDL_Surface *welcome =
    TTF_RenderText_Solid (fntbg, "Welcome to Quantum Minigolf", clrFg);
  SDL_Surface *select = TTF_RenderText_Solid (fntsml,
					      "Press <left> or <right> to select your track,",
					      clrFg);
  SDL_Surface *start =
    TTF_RenderText_Solid (fntsml, "<Enter> or <Space> to start a game",
			  clrFg);
  SDL_Surface *toggle =
    TTF_RenderText_Solid (fntsml, "<q> to toggle quantum mode", clrFg);
  string map = "<c> to switch to ";
  map.append ((cmap == cmapm) ? "colored " : "monochrome ");
  map.append ("colormap");
  SDL_Surface *switchmap = TTF_RenderText_Solid (fntsml, map.c_str (), clrFg);
  SDL_Surface *help =
    TTF_RenderText_Solid (fntsml, "<h> to toggle help overlay", clrFg);
  SDL_Surface *esc = TTF_RenderText_Solid (fntsml, "<Esc> to quit", clrFg);
  string qinfo = "(You are in ";
  qinfo.append (quantum ? "quantum" : "classical");
  qinfo.append (" mode)");
  SDL_Surface *info = TTF_RenderText_Solid (fntsml, qinfo.c_str (), clrFg);
  SDL_Surface *instr = TTF_RenderText_Solid (fntsml,
					     "Once the game is running, click, hold and release the left mouse button",
					     clrFg);
  SDL_Surface *iinstr = TTF_RenderText_Solid (fntsml,
					      "to kick the ball. Click once more to measure its position.",
					      clrFg);
  SDL_Rect rcDest = { 150, 50, 0, 0 };
  SDL_BlitSurface (welcome, NULL, bBuffer, &rcDest);
  rcDest.x = 150;
  rcDest.y += 2 * linesep;
  SDL_BlitSurface (select, NULL, bBuffer, &rcDest);
  rcDest.y += linesep;
  SDL_BlitSurface (start, NULL, bBuffer, &rcDest);
  rcDest.y += linesep;
  SDL_BlitSurface (toggle, NULL, bBuffer, &rcDest);
  rcDest.y += linesep;
  SDL_BlitSurface (info, NULL, bBuffer, &rcDest);
  rcDest.y += linesep;
  SDL_BlitSurface (switchmap, NULL, bBuffer, &rcDest);
  rcDest.y += linesep;
  SDL_BlitSurface (help, NULL, bBuffer, &rcDest);
  rcDest.y += linesep;
  SDL_BlitSurface (esc, NULL, bBuffer, &rcDest);

  rcDest.y = 240;
  SDL_BlitSurface (instr, NULL, bBuffer, &rcDest);
  rcDest.y += linesep;
  SDL_BlitSurface (iinstr, NULL, bBuffer, &rcDest);

  SDL_FreeSurface (welcome);
  SDL_FreeSurface (select);
  SDL_FreeSurface (start);
  SDL_FreeSurface (toggle);
  SDL_FreeSurface (switchmap);
  SDL_FreeSurface (info);
  SDL_FreeSurface (help);
  SDL_FreeSurface (esc);
  SDL_FreeSurface (instr);
  SDL_FreeSurface (iinstr);
}

// Blit
// Blit the contents of bBuffer to the screen.
// Note that all RenderXXX functions only draw to bBuffer. I.E.
// You have to call Blit before anything rendered gets visible
void
Renderer::Blit ()
{
  SDL_BlitSurface (bBuffer, NULL, screen, &rBuffer);
  SDL_UpdateRect (screen, 0, 0, 0, 0);
}

void
Renderer::SaveFrame (const char *fname)
{
  SDL_SaveBMP (bBuffer, fname);
}
