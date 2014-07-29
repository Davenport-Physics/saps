/*
*    Physics Sim
*
*    Copyright (C) 2014 Michael Davenport <Davenport.physics@gmail.com>
*
*
*   Permission is hereby granted, free of charge, to any person 
*   obtaining a copy of this software and associated documentation 
*   files (the "Software"), to deal in the Software without 
*   restriction, including without limitation the rights to use, 
*   copy, modify, merge, publish, distribute, sublicense, and/or 
*   sell copies of the Software, and to permit persons to whom 
*   the Software is furnished to do so, subject to the following 
*   conditions:
*
*   The above copyright notice and this permission notice shall be 
*   included in all copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
*   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
*   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
*   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
*   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
*   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
*   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR 
*   OTHER DEALINGS IN THE SOFTWARE.
*
*/

#ifndef	PARTICLES_H
#define	PARTICLES_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <pthread.h>

#include "constants.h"
#include "systemtime.h"
#include "electric.h"
#include "newtonian.h"
#include "mechanics.h"

#define ELECTRON_MASS    9.10938188e-31
#define ELECTRON_CHARGE -1.60217646e-19 

#define PROTON_MASS      1.67262158e-27
#define PROTON_CHARGE    1.60217646e-19

enum {
	
	ELECTRON = 0,
	PROTON,
	NEUTRON
	
};

struct location {
	
	double x, y, z;
	double radius;
	int done;
	
};

struct particle_attributes {
	
	int          index , type;
	double  mass  , charge; 
	
};

struct amount {
	
	int amountElectron, amountProton, amountNeutron;
	int total , ready;
	
};

extern struct location *electronLocations;
extern struct location *protonLocations;
extern struct location *neutronLocations;

extern struct amount *numParticles;

void init_particles(int numElectron, int numProton, pthread_cond_t *, pthread_cond_t *);
void quit_particles();

void *particles(void *);

#endif
