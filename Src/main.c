/*
*    Semi-accurate physics simulator (SAPS)
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
#define _GNU_SOURCE

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
	
	int retval;
	
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
		
		numElectron = 0;
		numProton   = 0;
		
		var.readyElectron = 0;
		var.readyProton   = 0;
		
		typeLength = 0;
		
		/*
		* systemFinished is a variable that each thread look at to keep them
		* running. systemFinished is either CONTINUE or FINISH.
		* 
		* Global variable defined and declared in systemtime.c/.h
		* 
		* */
		systemFinished = CONTINUE;
	
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
			
			retval = pthread_create(&secondary, NULL, particle_constructor, (void *)&var);			
			if (retval != 0) {
			
				printf("Constructor thread -> Error code: %d", retval);
			
			}
			
			retval = pthread_create( &event, NULL, engine_event, ( void *)0);
			if (retval != 0) {
				
				printf("Event thread -> Error code: %d", retval);
			
			}
			
			engine_run( &var , types , typeLength , runtime);
	
			retval = pthread_join(event, NULL);
			if (retval != 0) {
			
				printf("Event thread join -> Error code: %d", retval);
				
			}
			
			retval = pthread_join(secondary, NULL);
			if (retval != 0) {
			
				printf("Constructor thread join -> Error code: %d", retval);
				
			}
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
		types = NULL;
		
	}	
	
	return 0;
}
void *particle_constructor(void *n) {
	
	int x, y;
	
	int retval;
	
	/*
	 * I plan on having the contructor be able to set up different simulations.
	 * So this variable was set up as a prelude to that eventual code base.
	 * */
	int particle     = TRUE;
	
	/*
	 * This variable is used for mutex conditional statements. It's primary
	 * use is to prevent a thread from not receiving a mutex conditional
	 * signal, due to it not locking the mutex in time.
	 * 
	 * */
	int mutexcondition = FALSE;
	
	pthread_t electronThread[numElectron];
	pthread_t protonThread[numProton];
	pthread_t systemThread;
	
	pthread_mutex_t initmutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
	pthread_cond_t  initcond  = PTHREAD_COND_INITIALIZER;
	pthread_cond_t  startcond = PTHREAD_COND_INITIALIZER;
	
	struct enginevars *vars = (struct enginevars *)n;
	
	/*
	 * Holds index values, type of particle, mass of particle and charge
	 * of particle.
	 * */
	struct particle_attributes *electronAttributes;
	struct particle_attributes *protonAttributes;
	
	pthread_create( &systemThread, NULL, system_clock, ( void *)0 );
	
	if (particle == TRUE) {
		
		init_particles(numElectron, numProton, &initcond, &startcond, &mutexcondition);
		
		electronAttributes = ( struct particle_attributes * )malloc( numElectron * sizeof( struct particle_attributes ) );
		protonAttributes   = ( struct particle_attributes * )malloc( numProton * sizeof( struct particle_attributes ) );
		
		int numParticle    = numElectron;
		int *readyParticle = &vars->readyElectron;
		
		struct location *thisLocation              = electronLocations;
		struct particle_attributes *thisAttributes = electronAttributes;	
		
		int type = ELECTRON;
		
		double mass   = ELECTRON_MASS;
		double charge = ELECTRON_CHARGE;
		
		pthread_t *thisThread = electronThread;

		for (y = 0; y < 2; y++) {
			
			for (x = 0; x < numParticle; x++) {
		
				thisAttributes[x].index  = x;
				thisAttributes[x].type   = type;
				thisAttributes[x].mass   = mass;
				thisAttributes[x].charge = charge; 
				thisLocation[x].done     = 0;
				retval = pthread_create( &thisThread[x], NULL, particles , (void *)&thisAttributes[x] );
				
				if (retval != 0) {
				
					printf("Particle Thread -> Error code: %d", retval);
					
				}
		
				pthread_mutex_lock(&initmutex);
				
					pthread_cond_wait(&initcond, &initmutex);
				
				pthread_mutex_unlock(&initmutex);
				
				*readyParticle += 1;
	
			}
			
			numParticle    = numProton;
			readyParticle  = &vars->readyProton;
			thisLocation   = protonLocations;
			thisAttributes = protonAttributes;
			
			type   = PROTON;
			mass   = PROTON_MASS;
			charge = PROTON_CHARGE;
			
			thisThread = protonThread;
			
		}
		mutexcondition = TRUE;
		pthread_cond_broadcast(&startcond);
		
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


