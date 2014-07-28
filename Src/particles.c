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

#include "particles.h"


/*
 * 
 * Allows certain portions of the code to know what the particle
 * is interacting with.
 * 
 * Interactions
 * 
 * 0 - Electron/Electron
 * 1 - Electron/Proton
 * 2 - Proton/Proton
 * 3 - Proton/Electron
 * 
 * */
enum {
	EL_EL = 0,
	EL_PR,
	PR_PR,
	PR_EL
	
};

pthread_mutex_t ready;

//extern variables
struct location *electronLocations;
struct location *protonLocations;
struct location *neutronLocations;
struct amount   *numParticles;

static struct timespec hold;

inline float get_float();

void get_random_location(int index, struct location *thisParticle);
void compare_locations(int index, struct location *thisParticle);
void calculate_summation_force(int index, double thisCharge , struct movement *this , struct location * thisParticle);

void check_system();	 
void calculate_force( int index1 , int index2 , struct movement *this );
void calculate_acceleration( double mass , struct movement *this);
void calculate_velocity( double time , struct movement *this );
void calculate_displacement( double time , struct movement *this);

double calculate_distance(int index, int index2, struct location *thisParticle, struct location *thatParticle);


void init_particles(int numElectron, int numProton) {
	
	hold.tv_sec = 0;
	hold.tv_nsec = 250000000;
	
	//Holds location in 3 dimensions. Also has a variable to signal other threads it's ready to be interacted with.
	electronLocations = ( struct location * )malloc( numElectron * sizeof(struct location) );
	protonLocations = ( struct location * )malloc( numProton * sizeof(struct location) );
	
	//Holds the amount of each respective particle.
	numParticles = ( struct amount * )malloc( sizeof(struct amount) );
	
	numParticles->amountElectron = numElectron;
	numParticles->amountProton = numProton;
	numParticles->total = numElectron + numProton;

}

void quit_particles() {
	
	free(electronLocations);
	free(protonLocations);
	free(numParticles);
	
	numParticles	  = NULL;
	electronLocations = NULL;
	protonLocations   = NULL;

}

void *particles(void *att) {
	
	struct particle_attributes *attr = (struct particle_attributes * )att;
	struct movement current          = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	
	struct location *thisParticle;
	
	switch (attr->type) {
	
		case ELECTRON: thisParticle = electronLocations; break;
		case PROTON  : thisParticle = protonLocations;   break;
		
		default: printf("No type specified\n"); pthread_exit(EXIT_SUCCESS); break;
		
	}
	
	const int index = attr->index;
	//const int type  = attr->type;
	
	const double mass   = attr->mass;
	const double charge = attr->charge;
	
	get_random_location( index , thisParticle);
	compare_locations( index , thisParticle );
	
	thisParticle[index].radius = ( 1 + thisParticle[index].z ) / 22.22;
	thisParticle[index].done = 1;
	
	pthread_mutex_lock(&ready);
	
		numParticles->ready++;
	
	pthread_mutex_unlock(&ready);
	
	check_system();
	
	long double time = get_system_time();
	long double initialTime = time;
	
	long double temp;
	
	while (systemFinished == CONTINUE) {
		
		calculate_summation_force( index , charge , &current , thisParticle );
		calculate_acceleration( mass , &current );
		calculate_velocity( initialTime - time , &current);
		calculate_displacement( initialTime - time , &current );
		
		
		
		temp = new_position(thisParticle[index].x , current.displacementX , thisParticle[index].radius);
		if ( current.displacementX != temp ) {
			
			thisParticle[index].x = temp;
			
		
		} else {
		
			thisParticle[index].x += current.displacementX;
		
		}
		
		temp = new_position(thisParticle[index].y , current.displacementY , thisParticle[index].radius);
		if ( current.displacementY !=  temp ) {
		
			thisParticle[index].y = temp;
			
		} else {
		
			thisParticle[index].y += current.displacementY;
			
		}
		
		temp = new_position(thisParticle[index].z , current.displacementZ , thisParticle[index].radius);
		if ( current.displacementZ != temp ) {
		
			thisParticle[index].z = temp;
			
		} else {
		
			thisParticle[index].z += current.displacementZ;
		
		}
	
		thisParticle[index].radius = (1 + thisParticle[index].z) / 22.22;
	
		initialTime = time;
		time += get_system_time();
		nanosleep( &hold, NULL );
		
		
	}
	
	
	pthread_exit(EXIT_SUCCESS);
}

void get_random_location(int index, struct location *thisParticle) {
	
	thisParticle[index].x = get_float() - get_float();
	thisParticle[index].y = get_float() - get_float();
	thisParticle[index].z = get_float() - get_float();
	
}

void compare_locations( int index, struct location *thisParticle ) {
	
	int x, y;
	int amountParticles;
	
	int done = 0;
	
	struct location *thatParticle;
			
	while (done == 0) {
				
		done = 1;
		
		thatParticle    = electronLocations;
		amountParticles = numParticles->amountElectron;
		for (y = 0; y < 2 ; y++) {
			
			for ( x = 0; x <  amountParticles; x++) {
				
				if ( thisParticle == thatParticle && x == index ) {
					
					continue;
				
				} else if ( thisParticle[index].x == thatParticle[x].x &&
					thisParticle[index].y == thatParticle[x].y &&
				    thisParticle[index].z == thatParticle[x].z) {
					 
						get_random_location( index , thisParticle );	
					
						x    = 0;		
						done = 0;
				
				}
				
			}
			thatParticle    = protonLocations;
			amountParticles = numParticles->amountProton;
		
		}
			
	}
		
}


float get_float() {
	
	return ( float )rand()/( float )RAND_MAX;

}

void check_system() {
	
	
	while ( numParticles[0].ready != numParticles[0].total ) {
		
		nanosleep( &hold , NULL );
		
	}
	
}

void calculate_summation_force(int index, double thisCharge, struct movement *this, struct location *thisParticle) {
	
	int a, b;
	
	int num       = numParticles->amountElectron;
	double charge = ELECTRON_CHARGE;
	
	double sumx = 0, sumy = 0, sumz = 0;
	
	struct location *thatParticle = electronLocations;
	
	const double kq = COULOMBS_CONSTANT * thisCharge;
	
	for (a = 0; a < 2;a++) {
		
		for (b = 0; b < num;b++) {
			
			if (b == index && thisParticle == thatParticle) {
			
				continue;
				
			} else {
				
				sumx += summation_electric_field( charge , thisParticle[index].x - thatParticle[b].x);
				sumy += summation_electric_field( charge , thisParticle[index].y - thatParticle[b].y);
				sumz += summation_electric_field( charge , thisParticle[index].z - thatParticle[b].z);
				
			}
			
		}
		num          = numParticles->amountProton;
		charge       = PROTON_CHARGE;
		thatParticle = protonLocations;
	
	}
	this->force  = kq * sqrt( (sumx * sumx) + (sumy * sumy) + (sumz * sumz));
	this->forceX = kq * sumx;
	this->forceY = kq * sumy;
	this->forceZ = kq * sumz;
	

}
double calculate_distance(int index, int index2, struct location *thisParticle, struct location *thatParticle) {
	
	double x = thisParticle[index].x - thatParticle[index2].x;
	double y = thisParticle[index].y - thatParticle[index2].y;
	double z = thisParticle[index].z - thatParticle[index2].z;
	
	return sqrt( (x * x) + ( y * y) + ( z * z ) );

}

void calculate_acceleration( double mass , struct movement *this ) {
	
	this->acceleration  += acceleration_forceMass( this->force , mass ) - this->acceleration;
	this->accelerationX += acceleration_forceMass( this->forceX , mass ) - this->accelerationX;
	this->accelerationY += acceleration_forceMass( this->forceY , mass ) - this->accelerationY;
	this->accelerationZ += acceleration_forceMass( this->forceZ , mass ) - this->accelerationZ;
	

}
void calculate_velocity( double time , struct movement *this ) {
	
	this->velocity  += velocity_accelerationTime( this->acceleration , time ) - this->velocity;
	this->velocityX += velocity_accelerationTime( this->accelerationX , time ) - this->velocityX;
	this->velocityY += velocity_accelerationTime( this->accelerationY , time ) - this->velocityY;
	this->velocityZ += velocity_accelerationTime( this->accelerationZ , time ) - this->velocityZ;
	
}

void calculate_displacement( double time , struct movement *this ) {
	
	this->displacementX += metres_velocityTime( this->velocityX , time );
	this->displacementY += metres_velocityTime( this->velocityY , time );
	this->displacementZ += metres_velocityTime( this->velocityZ , time );
	
}

