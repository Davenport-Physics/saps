/*
 * main.c
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


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <pthread.h>

#include "particles.h"
#include "constants.h"
#include "engine.h"
#include "systemtime.h"

void set_mode();
void runloop(int x);
void keyboard(unsigned char key, int x, int y);
void display(void);

void *constructor(void *n);

int numElectron = 0, numProton = 0, numNeutron = 0;
int readyElectron = 0, readyProton = 0;

const float fps = 1000/60;

int main(int argc, char **argv)
{
	
	pthread_t secondary;
	pthread_t event;
	
	srand(time(NULL));
	
	printf("Number of electrons: ");
	scanf("%d" , &numElectron);
	
	printf("Number of protons: ");
	scanf("%d", &numProton);
	
	pthread_create(&secondary, NULL, constructor, (void *)0);
	
	engine_init();
	
	pthread_create( &event, NULL, engine_event, ( void *)0);
	
	engine_run(&readyElectron, &readyProton);
	
	
	systemFinished = 1;
	pthread_join(secondary, NULL);
	
	return 0;
}
void *constructor(void *n) {
	
	int x;
	
	//stops for a tenth of a second.
	struct timespec *hold = ( struct timespec * )malloc( sizeof(struct timespec) );
	hold[0].tv_sec = 0;
	hold[0].tv_nsec = 100000000;
	
	pthread_t electronThread[numElectron];
	pthread_t protonThread[numProton];
	pthread_t systemThread;
	
	//Holds the Index value of the current particle
	int *electronIndex = ( int * )malloc( numElectron * sizeof(int) );
	int *protonIndex = ( int * )malloc( numElectron * sizeof(int) );
	
	//Holds location in 3 dimensions. Also has a variable to signal other threads it's ready to be interacted with.
	electronLocations = ( struct location * )malloc( numElectron * sizeof(struct location) );
	protonLocations = ( struct location * )malloc( numProton * sizeof(struct location) );
	
	//Holds the amount of each respective particle.
	numParticles = ( struct amount * )malloc( sizeof(struct amount) );
	
	constants = ( struct data * )malloc( sizeof(struct data) );
	
	//Finished is the variable each thread look at to keep them running.
	systemFinished = 0;
	numParticles[0].amountElectron = numElectron;
	numParticles[0].amountProton = numProton;
	numParticles[0].amountNeutron = numNeutron;
	
	init_constants();
	init_particle_constants();
	
	pthread_create( &systemThread, NULL, system_clock, ( void *)0 );
	for (x = 0; x < numElectron; x++) {
		
		electronIndex[x] = x;
		electronLocations[x].done = 0;
		pthread_create( &electronThread[x], NULL, electron , (void *)&electronIndex[x] );
		
		while (electronLocations[x].done != 1 ) {
			
			nanosleep( hold , NULL );
		
		}
		readyElectron += 1;
	
	}
	for (x = 0;x < numProton; x++) {
		
		protonIndex[x] = x;
		protonLocations[x].done = 0;
		pthread_create( &protonThread[x], NULL , proton , (void *)&protonIndex[x] );
		
		while (protonLocations[x].done != 1 ) {
			
			nanosleep( hold , NULL );
		
		}
		readyProton += 1;
	
	}
	
	for (x = 0;x < readyElectron;x++) {
		
		pthread_join(electronThread[x], NULL);
	
	}
	for (x = 0;x < readyProton;x++) {
	
		pthread_join(protonThread[x], NULL);
		
	}
	pthread_join( systemThread, NULL );
	
	free( hold );
	free( electronIndex );
	free( protonIndex );
	free( numParticles );
	free( electronLocations );
	free( protonLocations );
	free( constants );
	
	
	pthread_exit(EXIT_SUCCESS);
	
}


