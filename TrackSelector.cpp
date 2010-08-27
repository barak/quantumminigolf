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

#include "TrackSelector.h"

// TrackSelector constructor: preload the desired track bitmaps
// and store them in the track list
TrackSelector::TrackSelector(Renderer *renderer, ClassicSimulator *csimulator)
{
	this->renderer = renderer;
	this->csimulator = csimulator;
	this->width = renderer->width;
	this->height = renderer->height;

	char line[80];
	char V[80];
	char hard[80];
	char soft[80];
	string fname;

	SDL_Surface *bmp;

	trackrecord *entry;

	help = true;

	/*read the config file, load the track bitmaps and store 
	them in the track vector*/
	FILE *conf = fopen("tracks/tracks.cfg", "r");
	//load an empty dummy track, if there is no config file present
	if(conf == NULL){
		cout << "Cannot open track config file" << endl;
			entry = new trackrecord;
			tracks.push_back(entry);
			entry->V = BlackTrack();
			entry->hard = BlackTrack();
			entry->soft = BlackTrack();
	}
	//config file present: load all tracks mentioned in the config file
	else{
		cout << "preloading tracks... " << endl;
		while(fgets(line, 79, conf)){
			for(int i=0; i<80-1; i++){
				hard[i] = ' '; soft[i] = ' ';
			}
			sscanf(line, "%s %s %s\n", V, hard, soft);
			fname = string("tracks/");
			fname.append(V);
			cout <<  fname << "...";
			bmp = (SDL_Surface *)SDL_LoadBMP(fname.c_str());

			/*if bitmap cannot be loaded or has the wrong format, skip it*/
			if(bmp == NULL || bmp->w != width || bmp->h != height){
				cout << "failed: no quantum track found" << endl;
				SDL_FreeSurface(bmp); 
				bmp = BlackTrack();
			}
			entry = new trackrecord;
			tracks.push_back(entry);
			entry->V = SDL_ConvertSurface(bmp, renderer->screen->format, SDL_SWSURFACE);

			fname = string("tracks/");
			fname.append(hard);
			bmp = (SDL_Surface *)SDL_LoadBMP(fname.c_str());
			/*if bitmap cannot be loaded or has the wrong format, skip it*/
			if(bmp == NULL || bmp->w != width || bmp->h != height){
				cout << fname << ":failed to load hardcore potential " << endl;
				SDL_FreeSurface(bmp); 
				bmp = BlackTrack();
			}
			entry->hard = SDL_ConvertSurface(bmp, renderer->screen->format, SDL_SWSURFACE);

			fname = string("tracks/");
			fname.append(soft);
			bmp = (SDL_Surface *)SDL_LoadBMP(fname.c_str());
			/*if bitmap cannot be loaded or has the wrong format, skip it*/
			if(bmp == NULL || bmp->w != width || bmp->h != height){
				cout << fname << ":failed to load softcore potential" << endl;
				SDL_FreeSurface(bmp); 
				bmp = BlackTrack();
			}
			entry->soft = SDL_ConvertSurface(bmp, renderer->screen->format, SDL_SWSURFACE);

			cout << "done" << endl;
		}
	}
	trackiterator = tracks.begin();
	renderer->V = (*trackiterator)->V;
	csimulator->hard = (*trackiterator)->hard;
	csimulator->soft = (*trackiterator)->soft;
	renderer->RenderTrack();
	renderer->Blit();
}

TrackSelector::~TrackSelector(void)
{
	list<trackrecord *>::iterator l;
	for(l=tracks.begin(); l!=tracks.end(); ++l){
		SDL_FreeSurface((*l)->V);
		SDL_FreeSurface((*l)->hard);
		SDL_FreeSurface((*l)->soft);
		delete *l;
	}
}


//TrackSelector::GetTrack 
// take control of the renderer and generate the track choice menu
// release control of the renderer only when the user has chosen a track
// or quit the game
int TrackSelector::GetTrack(bool *quantum){
    SDL_Event dummyevent;

	int finished = 0;

	SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);  
	SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);  

	renderer->V = (*trackiterator)->V;
	renderer->RenderTrack();
	if(help)renderer->RenderMenu(*quantum);
	renderer->Blit();

	while(!finished){
		if(SDL_PollEvent(&dummyevent)==0)
			continue;

		switch(dummyevent.type){
			case SDL_KEYDOWN:
				switch(dummyevent.key.keysym.sym){
					case SDLK_ESCAPE: //Esc - quit
						return 0;
						break;
					case SDLK_RETURN: // RET or Space - start a game on the current track
					case SDLK_SPACE:  
						SDL_EventState(SDL_MOUSEBUTTONUP, SDL_ENABLE);  
						SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_ENABLE);  
						return 1;
						break;
					case SDLK_LEFT: // left or right - cycle tracks
						if(trackiterator == tracks.begin()){
							trackiterator = tracks.end();
							--trackiterator;
						}
						else
							--trackiterator;
						renderer->V = (*trackiterator)->V;
						csimulator->hard = (*trackiterator)->hard;
						csimulator->soft = (*trackiterator)->soft;
						renderer->RenderTrack();
						if(help)renderer->RenderMenu(*quantum);
						renderer->Blit();
						break;
					case SDLK_RIGHT: 
						++trackiterator;
						if(trackiterator == tracks.end())
							trackiterator = tracks.begin();
						renderer->V = (*trackiterator)->V;
						csimulator->hard = (*trackiterator)->hard;
						csimulator->soft = (*trackiterator)->soft;
						renderer->RenderTrack();
						if(help)renderer->RenderMenu(*quantum);
						renderer->Blit();
						break;
					case SDLK_q: // q - toggle quantum mode
						*quantum = !*quantum;
						renderer->RenderTrack();
						if(help)renderer->RenderMenu(*quantum);
						renderer->Blit();
						break;
					case SDLK_h: // h - toggle help overlay
						help = !help;
						renderer->RenderTrack();
						if(help)renderer->RenderMenu(*quantum);
						renderer->Blit();
						break;
					case SDLK_c: // c - toggle color map
					    renderer->ToggleCmap();
					    renderer->RenderTrack();
					    if(help)renderer->RenderMenu(*quantum);
					    renderer->Blit();
					    break;
				}
				break;
			default:
				renderer->Blit();
				break;
		}
	}
}

//BlackTrack -- generate an empty track surface
SDL_Surface * TrackSelector::BlackTrack(void){
	SDL_Surface *result = NULL;

	result = SDL_CreateRGBSurface( SDL_SWSURFACE, width, 
				height, 
				renderer->screen->format->BitsPerPixel,
				renderer->screen->format->Rmask,
				renderer->screen->format->Gmask,
				renderer->screen->format->Bmask,
				renderer->screen->format->Amask);
	if (result == NULL){
		cout << "failed to create software surface" << endl;
		exit(1);
	}
	Uint32 *pdata = ((Uint32 *)(result->pixels));
	for(int i=0; i<width; i++)
		for(int j=0; j<height; j++){
			pdata[j*width+i] = 0;
		}
	return result;
}

