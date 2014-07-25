/*
 * particles.c
 * 
 * Copyright 2013 Michael Davenport <Davenport.physics@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#ifndef	PARTICLES_H
#define	PARTICLES_H

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

#define ELECTRON 0
#define PROTON 1
#define NEUTRON 2

#define ELECTRON_MASS    9.10938188e-31
#define ELECTRON_CHARGE -1.60217646e-19 

#define PROTON_MASS      1.67262158e-27
#define PROTON_CHARGE    1.60217646e-19

struct location {
	
	float x, y, z;
	float radius;
	int done;
	
};

struct particle_attributes {
	
	int          index , type;
	long double  mass  , charge; 
	
};

struct amount {
	
	int amountElectron, amountProton, amountNeutron;
	int total , ready;
	
};

extern struct location *electronLocations;
extern struct location *protonLocations;
extern struct location *neutronLocations;

extern struct amount *numParticles;

void init_particle(int numElectron, int numProton);
void quit_particle();

void *proton( void * );
void *electron( void * );
void *particles(void *);

#endif
