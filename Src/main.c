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

void *particle_constructor(void *n);

static struct timespec hold;

static int numElectron = 0, numProton = 0;

int main(int argc, char **argv)
{
	srand(time(NULL));
	
	pthread_t secondary;
	pthread_t event;
	
	char command[100] = {'\0'};
	
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
	
	struct enginevars var;
	
	/*
	 * *types is a arbitrary length variable that tells the engine which
	 * functions to call, in order to render the correct simulation.
	 * 
	 * typeLength is used by the engine, as a reference to how many
	 * rendering jobs it has to do per frame.
	 * 
	 * */
	int *types        = NULL;	
	int typeLength;
	while (1) {
		
		var.readyElectron = 0;
		var.readyProton   = 0;
		
		typeLength = 0;
	
		printf("Type: ");
		scanf("%s", command);
	
		if (strcmp( command , "particles" ) == 0) {
			
			types = ( int * )malloc( sizeof( int ) );
			
			types[0]   = 1;
			typeLength = 1;
		
			printf("Number of electrons: ");
			scanf("%d" , &numElectron);
			printf("Number of protons: ");
			scanf("%d", &numProton);
	
			engine_init();
			pthread_create(&secondary, NULL, particle_constructor, (void *)&var);
			pthread_create( &event, NULL, engine_event, ( void *)0);
			engine_run( &var , types , typeLength , runtime);
	
			pthread_join(event, NULL);
			pthread_join(secondary, NULL);
			engine_quit();
		
		} else if ( strcmp( command , "ball" ) == 0 ) {
			
			types = ( int * )malloc( sizeof( int ) );
		
			types[0]   = 2;
			typeLength = 1;
			
			engine_init();
			pthread_create(&event , NULL, engine_event, (void *)0);
			engine_run(&var , types , typeLength , runtime);
			
			pthread_join(event, NULL);
			engine_quit();
	
		} else if ( strcmp( command , "quit" ) == 0 ) {
			
			break;
			
		} else if ( strcmp( command , "help") == 0) {
			
			puts("\nparticles			Initiates particle simulation\n"
				 "quit				Quits the program\n");
			
		}else {
			
			printf("That is not a correct type of operation\n");
			
		}
		
		free( types );
		
	}	
	
	return 0;
}
void *particle_constructor(void *n) {
	
	int x, y;
	
	int particle = TRUE;
	
	pthread_t electronThread[numElectron];
	pthread_t protonThread[numProton];
	pthread_t systemThread;
	
	struct enginevars *vars = (struct enginevars *)n;
	
	/*
	 * Holds index values, type of particle, mass of particle and charge
	 * of particle.
	 * */
	struct particle_attributes *electronAttributes;
	struct particle_attributes *protonAttributes;
	
	/*
	 * systemFinished is a variable that each thread look at to keep them
	 * running. systemFinished is either CONTINUE or FINISH.
	 * 
	 * Global variable defined and declared in systemtime.c/.h
	 * 
	 * */
	systemFinished = CONTINUE;
	
	pthread_create( &systemThread, NULL, system_clock, ( void *)0 );
	
	if (particle == TRUE) {
		
		init_particles(numElectron, numProton);
		
		electronAttributes = ( struct particle_attributes * )malloc( numElectron * sizeof( struct particle_attributes ) );
		protonAttributes   = ( struct particle_attributes * )malloc( numProton * sizeof( struct particle_attributes ) );
		
		int numParticle   = numElectron;
		int *readyParticle = &vars->readyElectron;
		
		struct location *thisLocation              = electronLocations;
		struct particle_attributes *thisAttributes = electronAttributes;	
		
		int type = ELECTRON;
		
		long double mass   = ELECTRON_MASS;
		long double charge = ELECTRON_CHARGE;
		
		pthread_t *thisThread = electronThread;

		for (y = 0; y < 2; y++) {
			
			for (x = 0; x < numParticle; x++) {
		
				thisAttributes[x].index  = x;
				thisAttributes[x].type   = type;
				thisAttributes[x].mass   = mass;
				thisAttributes[x].charge = charge; 
				thisLocation[x].done     = 0;
				pthread_create( &thisThread[x], NULL, particles , (void *)&thisAttributes[x] );
		
				while (electronLocations[x].done != 1 ) {
			
					nanosleep( &hold , NULL );
		
				}
				*readyParticle += 1;
	
			}
			
			numParticle    = numProton;
			readyParticle  = &vars->readyProton;
			thisLocation   = protonLocations;
			thisAttributes = protonAttributes;
			
			mass   = PROTON_MASS;
			charge = PROTON_CHARGE;
			
			thisThread = protonThread;
			
		}
		
	}
	
	if (particle == TRUE) {
	
		int numParticle = numElectron;
		
		pthread_t *thisThread = electronThread;
		
		for (y = 0; y < 2; y++) {
			
			for (x = 0; x < numParticle;x++) {
			
				pthread_join( thisThread[x] , NULL );
		
			}
			
			numParticle = numProton;
			
			thisThread = protonThread;
			
		}
		
		quit_particles();
		
		free( electronAttributes );
		free( protonAttributes );
	
		electronAttributes	  = NULL;
		protonAttributes	  = NULL;
		
	}
	
	pthread_join( systemThread, NULL );
	
	pthread_exit(EXIT_SUCCESS);
	
}


