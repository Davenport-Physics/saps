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
*   	The above copyright notice and this permission notice shall be 
*   	included in all copies or substantial portions of the Software.
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
	
	enum engine_runtime runtime = NORMAL;
	
	if (argc > 1) {
		
		int x;
		for ( x = 1; x < argc; x++ ) {
		
			if (strcasecmp(argv[x],"debug") == 0) {
			
				runtime = DEBUG;
				
			} else {
			
				printf("%s is not a valid command" , argv[x]);
				
			}
		
		}
		
	}
	
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
			engine_run( &var , types , runtime);
	
			pthread_join(event, NULL);
			pthread_join(secondary, NULL);
			engine_quit();
		
		} else if ( strcmp( type , "ball" ) == 0 ) {
		
			//engine_run();
	
		} else if ( strcmp( type , "quit" ) == 0 ) {
			
			break;
			
		} else if ( strcmp(type , "help") == 0) {
			
			puts("\nparticles			Initiates particle simulation\n"
				 "quit				Quits the program\n");
			
		}else {
			
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
		pthread_create( &electronThread[x], NULL, particles , (void *)&electronAttributes[x] );
		
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
		protonLocations[x].done    = 0;
		pthread_create( &protonThread[x], NULL , particles , (void *)&protonAttributes[x] );
		
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


