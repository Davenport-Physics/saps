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

struct location {
	
	float x, y, z;
	float radius;
	int done;
	
};
struct amount {
	
	int amountElectron, amountProton, amountNeutron;
	int total , ready;
	
};

struct location *electronLocations;
struct location *protonLocations;
struct location *neutronLocations;

struct amount *numParticles;

void init_particle();
void quit_particle();

void *proton( void * );
void *electron( void * );
void *nuetron( void * );

#endif
