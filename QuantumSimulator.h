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


//QuantumSimulator - the class implementing the numerical solution of the
// time-dependent Schrödinger equation. 
// The solution is based on a time-splitting scheme.
// The propagation is performed successively in momentum and position space
// To propagate in momentum space, the wave function is fourier-transformed.
//

#ifdef WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include <time.h>
#include <malloc.h>
#include <math.h>
#include <fftw3.h>
#include <SDL.h>

class QuantumSimulator
{
public:
	QuantumSimulator(int width, int height, float dt);

	void BuildPositionPropagator(SDL_Surface *V);
	void BuildMomentumPropagator();

	float PropagatePosition(float quench);
	void PropagateMomentum();

	// return the result of a position measurement on psi
	void PositionMeasurement(int *x, int *y);

	// GenGauss - initialize psi with a gaussian wavepacket of width w, 
	// centered around cx and cy in position and around kx and ky in momentum space
	void GenGauss(int cx, int cy, float kx, float ky, float w );
	void ClearWave(void);

	fftwf_complex *psi; // the complex wavefunction
	fftwf_complex *xprop; // the propagator in position space

public:
	~QuantumSimulator(void);

private:
	fftwf_complex *prop; // the propagator in momentum space

	fftwf_plan fft, ifft; // plans for the Fourier transformations 
							// into momentum and position space
	float dt;			// the timestep
	int width, height;
	float GaussNorm;		// Norm of the wave packet after initialization

};
