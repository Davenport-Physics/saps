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

void *constructor(void *n);

static struct timespec hold;

static int numElectron = 0, numProton = 0;

int main(int argc, char **argv)
{
	srand(time(NULL));
	
	pthread_t secondary;
	pthread_t event;
	
	int types[3];	
	char type[100] = {'\0'};
	
	struct enginevars var;
	
	hold.tv_sec  = 0;
	hold.tv_nsec = 100000000;
	
	while (1) {
		
		var.readyElectron = 0;
		var.readyProton   = 0;
		
		types[0] = 0;
		types[1] = 0;
		types[2] = 0;
	
		printf("Type: ");
		scanf("%s", type);
	
		if (strcmp( type , "particles" ) == 0) {
			
			types[0] = 1;
		
			printf("Number of electrons: ");
			scanf("%d" , &numElectron);
			printf("Number of protons: ");
			scanf("%d", &numProton);
	
			engine_init();
			pthread_create(&secondary, NULL, constructor, (void *)&var);
			pthread_create( &event, NULL, engine_event, ( void *)0);
			engine_run( &var , types );
	
			pthread_join(event, NULL);
			pthread_join(secondary, NULL);
			engine_quit();
		
		} else if ( strcmp( type , "ball" ) == 0 ) {
		
			//engine_run();
	
		} else if ( strcmp( type , "quit" ) == 0 ) {
			
			break;
			
		} else {
			
			printf("That is not a correct type of operation\n");
			
		}
		
	}	
	
	return 0;
}
void *constructor(void *n) {
	
	int x;
	
	pthread_t electronThread[numElectron];
	pthread_t protonThread[numProton];
	pthread_t systemThread;
	
	struct enginevars *vars = (struct enginevars *)n;
	
	//Holds the Index value of the current particle
	struct particle_attributes *electronAttributes = ( struct particle_attributes * )malloc( numElectron * sizeof( struct particle_attributes ) );
	struct particle_attributes *protonAttributes   = ( struct particle_attributes * )malloc( numProton * sizeof( struct particle_attributes ) );
	
	//systemFinished is the variable each thread look at to keep them running.
	systemFinished = CONTINUE;
	init_particles(numElectron, numProton);
	
	pthread_create( &systemThread, NULL, system_clock, ( void *)0 );
	for (x = 0; x < numElectron; x++) {
		
		electronAttributes[x].index  = x;
		electronAttributes[x].type   = ELECTRON;
		electronAttributes[x].mass   = ELECTRON_MASS;
		electronAttributes[x].charge = ELECTRON_CHARGE; 
		electronLocations[x].done    = 0;
		pthread_create( &electronThread[x], NULL, electron , (void *)&electronAttributes[x] );
		
		while (electronLocations[x].done != 1 ) {
			
			nanosleep( &hold , NULL );
		
		}
		vars->readyElectron += 1;
	
	}
	for (x = 0;x < numProton; x++) {
		
		protonAttributes[x].index  = x;
		protonAttributes[x].type   = PROTON;
		protonAttributes[x].mass   = PROTON_MASS;
		protonAttributes[x].charge = PROTON_CHARGE;
		protonLocations[x].done = 0;
		pthread_create( &protonThread[x], NULL , proton , (void *)&protonAttributes[x] );
		
		while (protonLocations[x].done != 1 ) {
			
			nanosleep( &hold , NULL );
		
		}
		vars->readyProton += 1;
	
	}
	
	for (x = 0;x < numElectron;x++) {
		
		pthread_join(electronThread[x], NULL);
	
	}
	for (x = 0;x < numProton;x++) {
	
		pthread_join(protonThread[x], NULL);
		
	}
	pthread_join( systemThread, NULL );
	quit_particles();
	
	free( electronAttributes );
	free( protonAttributes );
	
	electronAttributes	  = NULL;
	protonAttributes	  = NULL;
	
	pthread_exit(EXIT_SUCCESS);
	
}


