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
struct location *this_that(int type, int count );

void check_system();
void calculate_difference( int index1 , int index2 , long double *x , long double *y , long double *z , int type );		 
void calculate_force( int index1 , int index2 , struct movement *this );
void calculate_acceleration( long double mass , struct movement *this );
void calculate_velocity( int index1, int index2 , long double time , struct movement *this );
void calculate_displacement( int index1 , int index2 , long double time , struct movement *this);
void calculate_components( long double x , long double y , long double z , struct movement *this , int type );


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
	const int type  = attr->type;
	
	const long double mass   = attr->mass;
	
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
	
	int x;
	long double temp;
	
	int amountParticle;
	while (systemFinished == CONTINUE) {
		
		amountParticle = 0;
		
		switch (type) {
		
			case ELECTRON: 
			
				current.type = EL_EL;
				amountParticle = numParticles->amountElectron;
			
			break;
			case PROTON  : 
			
				current.type = PR_PR;
				amountParticle = numParticles->amountProton;
			
			break;  
			
		}
		for (x = 0; x < amountParticle; x++) {
			
			if ( index == x ) {
			
				continue;
				
			} else {
			
				calculate_force( index, x , &current );
				calculate_acceleration( mass , &current );
				calculate_velocity( index , x ,  time - initialTime , &current );					
				calculate_displacement( index , x , time - initialTime , &current );
			
			}
		
		}
		amountParticle = 0;
		switch (type) {
		
			case ELECTRON: 
			
				current.type = EL_PR;
				amountParticle = numParticles->amountProton;
			
			break;
			case PROTON  : 
			
				current.type = PR_EL;
				amountParticle = numParticles->amountElectron;
			
			break;  
			
		}
		for (x = 0; x < amountParticle; x++) {
			
			if ( index == x ) {
			
				continue;
				
			} else {
			
				calculate_force( index, x , &current );
				calculate_acceleration( mass , &current );
				calculate_velocity( index , x ,  time - initialTime , &current );					
				calculate_displacement( index , x , time - initialTime , &current );
			
			}
		
		}
		
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

struct location *this_that(int type, int count) {
	
	switch (type) {
	
		case EL_EL:
		
			return electronLocations;
		
		break;
		
		case EL_PR:
		
			if (count == 0)
				return electronLocations;
			else
				return protonLocations;
			
			
		break;
		
		case PR_PR:
		
			return protonLocations;
			
		break;
		
		case PR_EL:
		
			if (count == 0)
				return protonLocations;
			else
				return electronLocations;
			
		break;
		default: return NULL; break;
		
	}
	
}

inline float get_float() {
	
	return ( float )rand()/( float )RAND_MAX;

}

void check_system() {
	
	
	while ( numParticles[0].ready != numParticles[0].total ) {
		
		nanosleep( &hold , NULL );
		
	}
	
}

void calculate_difference( int index1 , int index2 , long double *x , long double *y , long double *z , int type ) {
	
	
	struct location *thisParticle = this_that(type , 0);
	struct location *thatParticle = this_that(type , 1);
	
	*x = thisParticle[index1].x - thatParticle[index2].x;
	*y = thisParticle[index1].y - thatParticle[index2].y;
	*z = thisParticle[index1].z - thatParticle[index2].z;

}

void calculate_force( int index1 , int index2 ,  struct movement *this ) {
	
	
	//Because interaction at the nano level is too quickly to see.
	long double scale = 50;
	
	long double x , y , z;
	long double distance;
	
	long double first, second;
	
	
	calculate_difference( index1 , index2 , &x , &y , &z , this->type );
		
	distance = sqrtl( ( x * x ) + ( y * y ) + ( z * z ) );
	
	switch ( this->type ) {
		
		case PR_PR:
		case EL_EL: first = ELECTRON_CHARGE; second = ELECTRON_CHARGE; break;
		
		case PR_EL:
		case EL_PR: first = ELECTRON_CHARGE; second = PROTON_CHARGE; break;
		
		default   : first = 0; second = 0; break;
	
	
	}
	this->force += force_kqqR2( first , second , 
										distance * scale ) - this->force;

}

void calculate_acceleration( long double mass , struct movement *this ) {
	
	this->acceleration += acceleration_forceMass( this->force , mass ) - this->acceleration;

}
void calculate_velocity( int index1 , int index2 ,  long double time , struct movement *this ) {
	
	long double x , y , z;
	
	calculate_difference( index1 , index2 , &x , &y , &z , this->type );
	
	this->velocity += velocity_accelerationTime( this->acceleration, time ) - this->velocity;
	
	calculate_components( x , y , z , this , VELOCITY);
	
}

void calculate_displacement( int index1 , int index2 , long double time , struct movement *this ) {
	
	long double scale = 1;
	
	struct location *thisParticle = this_that( this->type , 0 );
	struct location *thatParticle = this_that( this->type , 1 );
	
	switch ( this->type ) {
		
		case PR_PR:
		case EL_EL:
		
			if ( thisParticle[index1].x > thatParticle[index2].x && thisParticle[index1].x < 1.0) {
			
				this->displacementX += ( metres_velocityTime( this->velocityX , time ) / scale );
		
			} else if ( thisParticle[index1].x == thatParticle[index2].x ) {
			
			
		
			} else {
			
				this->displacementX -=  ( metres_velocityTime( this->velocityX , time ) / scale );
		
			}
		
			if ( thisParticle[index1].y > thatParticle[index2].y && thisParticle[index1].y < 1.0 ) {
			
				this->displacementY += ( metres_velocityTime( this->velocityY , time ) / scale );
		
			} else if ( thisParticle[index1].y == thatParticle[index2].y ) {
			
				
		
			} else {
			
				this->displacementY -= ( metres_velocityTime( this->velocityY , time ) / scale );
				
			}
		
			if (thisParticle[index1].z > thatParticle[index2].z && thisParticle[index1].z < 1.0 ) {
			
				this->displacementZ += ( metres_velocityTime( this->velocityZ , time ) / scale );
		
			} else if ( thisParticle[index1].z == thatParticle[index2].z ) {
			
			
		
			} else {
			
				this->displacementZ -= ( metres_velocityTime( this->velocityZ , time ) / scale );
			
			}
			
		break;
		
		case PR_EL:
		case EL_PR:
		
			if ( thisParticle[index1].x > thatParticle[index2].x && thisParticle[index1].x > -1.0 ) {
				
				this->displacementX += ( metres_velocityTime( this->velocityX , time ) );
			
			} else if ( thisParticle[index1].x == thatParticle[index2].x ) {
				
				
				
			} else {
				
				this->displacementX -= ( metres_velocityTime( this->velocityX , time ) );
			
			}
			
			if ( thisParticle[index1].y > thatParticle[index2].y && thisParticle[index1].y > -1.0 ) {
				
				this->displacementY += ( metres_velocityTime( this->velocityY , time ) );
			
			} else if ( thisParticle[index1].y == thatParticle[index2].y ) {
				
			
				
			} else {
				
				this->displacementY -= ( metres_velocityTime( this->velocityY , time ) );
			
			}
			
			if ( thisParticle[index1].z > thisParticle[index2].z && thisParticle[index1].z > -1.0 ) {
				
				this->displacementZ += ( metres_velocityTime( this->velocityZ , time ) );
			
			} else if ( thisParticle[index1].z == thisParticle[index2].z ) {
				
				
			
			} else {
				
				this->displacementZ -= ( metres_velocityTime( this->velocityZ , time ) );
			
			}
		
		break;
		
	}
	
}
void calculate_components( long double x , long double y , long double z , struct movement *this , int type ) {
	
	long double xytheta = atan( y / x ), yztheta = atan( z / y );
	
	switch ( type ) {
		
		case FORCE: 
		
			this->forceX += ( this->force * cos( xytheta ) );
			this->forceY += ( this->force * sin( xytheta ) );
			this->forceZ += ( this->force * sin( yztheta ) );
			 
		break;
		
		case ACCELERATION:
		
			this->accelerationX += ( this->acceleration * cos( xytheta ) );
			this->accelerationY += ( this->acceleration * sin( xytheta ) );
			this->accelerationZ += ( this->acceleration * sin( yztheta ) );
		
		break;
		
		case VELOCITY:
		
			this->velocityX += ( this->velocity * cos( xytheta ) );
			this->velocityY += ( this->velocity * sin( xytheta ) );
			this->velocityZ += ( this->velocity * sin( yztheta ) );
		
		break;
	
	
	} 


}
