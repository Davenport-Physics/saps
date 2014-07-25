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


#include "particles.h"

#define EL_EL 0
#define EL_PR 1
#define PR_PR 2
#define PR_EL 3

/*
 * 
 * int type allows certain portions of the code to know what the particle
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
pthread_mutex_t ready;

//extern variables
struct location *electronLocations;
struct location *protonLocations;
struct location *neutronLocations;
struct amount   *numParticles;

static struct timespec hold;

inline float get_float();

void get_random_location(int index, int type);
void compare_locations(int index, int type);
struct location *this_that(int type, int count );

void check_system();
void calculate_difference( int index1 , int index2 , long double *x , long double *y , long double *z , int type );		 
void calculate_force( int index1 , int index2 , struct movement *this );
void calculate_acceleration( long double mass , struct movement *this );
void calculate_velocity( int index1, int index2 , long double time , struct movement *this );
void calculate_displacement( int index1 , int index2 , long double time , struct movement *this);
void calculate_components( long double x , long double y , long double z , struct movement *this , int type );

void init_particle(int numElectron, int numProton) {
	
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
		case PROTON:   thisParticle = protonLocations;   break;
		
	}
	
	const int index = attr->index;
	const int type  = attr->type;
	
	get_random_location( index , type );
	compare_locations( index , type );
	
	thisParticle[index].radius = ( 1 + thisParticle[index].z ) / 22.22;
	thisParticle[index].done = 1;
	
	pthread_mutex_lock(&ready);
	
		numParticles->ready += 1;
	
	pthread_mutex_unlock(&ready);
	
	check_system();
	
	long double time = get_system_time();
	long double initialTime = time;
	
	while (systemFinished == CONTINUE) {
		
		int x;
	
		long double temp;
		
		switch (type) {
		
			case ELECTRON: current.type = EL_EL; break;
			case PROTON  : current.type = EL_PR; break;  
			
		}
		for (x = 0; x < numParticles->amountElectron; x++) {
			
			if ( index == x && current.type == EL_EL ) {
			
				continue;
				
			} else {
			
				calculate_force( index, x , &current );
				calculate_acceleration( ELECTRON_MASS , &current );
				calculate_velocity( index , x ,  time - initialTime , &current );					
				calculate_displacement( index , x , time - initialTime , &current );
			
			}
		
		}
		if (type == PROTON) {
		
			for (x = 0; x < numParticles->amountProton; x++) {
			
				if ( index == x ) {
				
					continue;
					
				} else {
					
					calculate_force( index , x , &current );
					calculate_acceleration( PROTON_MASS , &current );
					calculate_velocity( index , x , time - initialTime , &current );
					calculate_displacement( index , x , time - initialTime , &current );
					
				}
				
			}
			
		}
		
	}
	
	
	pthread_exit(EXIT_SUCCESS);
}

void *electron( void *loc ) {

	int x = 0;
	
	long double temp;
	
	int *index = (int *)loc;
	struct movement current = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	
	get_random_location(*index, ELECTRON);
	
	//Justs checks to see if no electron is holding the same position.
	//The new location is based on probability that it won't be there again.
	for ( x = 0;x < *index; x++) {
			
		if ( electronLocations[*index].x == electronLocations[x].x && 
			 electronLocations[*index].y == electronLocations[x].y &&
			 electronLocations[*index].z == electronLocations[x].z ) {
					
				
				get_random_location( *index , ELECTRON);
				x = 0;
				
		}
		
	}
	electronLocations[*index].radius = (1 + electronLocations[*index].z) / 33.33;
	electronLocations[*index].done = 1;
	
	pthread_mutex_lock(&ready);
	
		numParticles[0].ready += 1;
	
	pthread_mutex_unlock(&ready);
	
	check_system();
	
	//Math can only be done two numbers at a time.
	
	long double time = get_system_time();
	long double initialTime = time;
	
	/*
	 * 
	 * This extensive while loop allows for particle interactions.
	 * 
	 * It compares the position values of other memory locations in in the multiple array's
	 * 
	 * */
	while ( systemFinished == CONTINUE ) {
	
		current.type = EL_EL;
		
		for ( x = 0;x < numParticles[0].amountElectron;x++ ) {
		
			if ( x == *index ) {
				
				continue;
			
			} else {
				
				calculate_force( *index, x , &current );
				calculate_acceleration( ELECTRON_MASS , &current );
				calculate_velocity( *index , x ,  time - initialTime , &current );					
				calculate_displacement( *index , x , time - initialTime , &current );
			
			}
		
		}
		
		current.type = EL_PR;
		
		for ( x = 0;x < numParticles[0].amountProton;x++ ) {
			
			
			calculate_force( *index, x, &current );
			calculate_acceleration( ELECTRON_MASS , &current );
			calculate_velocity( *index, x , time - initialTime , &current );
			calculate_displacement( *index , x , time - initialTime , &current );
		
		}
		
		/* 
		 * This code checks to see whether the particles have escaped
		 * outside of the boundary or are on the verge of doing so. In
		 * the case that they are, the code will set them as close to
		 * the boundary as possible, using the radius of the particle as
		 * a reference distance.
		 * 
		 * {@ 
		 * */
		temp = new_position(electronLocations[*index].x , current.displacementX , electronLocations[*index].radius);
		if ( current.displacementX != temp ) {
			
			electronLocations[*index].x = temp;
			
		
		} else {
		
			electronLocations[*index].x += current.displacementX;
		
		}
		
		temp = new_position(electronLocations[*index].y , current.displacementY , electronLocations[*index].radius);
		if ( current.displacementY !=  temp ) {
		
			electronLocations[*index].y = temp;
			
		} else {
		
			electronLocations[*index].y += current.displacementY;
			
		}
		
		temp = new_position(electronLocations[*index].z , current.displacementZ , electronLocations[*index].radius);
		if ( current.displacementZ != temp ) {
		
			electronLocations[*index].z = temp;
			
		} else {
		
			electronLocations[*index].z += current.displacementZ;
		
		}
		/* @} */
	
		electronLocations[*index].radius = (1 + electronLocations[*index].z) / 33.33;
	
		initialTime = time;
		time += get_system_time();
		nanosleep( &hold, NULL );
	
	}
	
	pthread_exit(EXIT_SUCCESS);
	
}

void *proton( void *loc ) {
	
	
	int x = 0;
	float temp;
	
	int *index = (int *)loc;
	
	struct movement current = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	
	for ( x = 0;x < numParticles[0].amountElectron;x++ ) {
		
		if ( protonLocations[*index].x == electronLocations[x].x &&
			 protonLocations[*index].y == electronLocations[x].y && 
			 protonLocations[*index].z == electronLocations[x].z) {
				 
				
				get_random_location( *index , PROTON );
				x = 0;
		
		}
		
	}
		
	for ( x = 0; x < *index;x++ ) {
			
		if ( protonLocations[*index].x == protonLocations[x].x &&
			 protonLocations[*index].y == protonLocations[x].y &&
			 protonLocations[*index].z == protonLocations[x].z) {
					 
				get_random_location( *index , PROTON );	
				x = 0;
				
		}
			
	}
	
	protonLocations[*index].radius = ( 1 + protonLocations[*index].z ) / 22.22;
	protonLocations[*index].done = 1;
	
	pthread_mutex_lock(&ready);
	
		numParticles[0].ready += 1;
	
	pthread_mutex_unlock(&ready);
	
	check_system();
	
	
	long double time = get_system_time(); 
	long double initialTime = time;
	
	while ( systemFinished == CONTINUE ) {
		
		current.type = PR_EL;
		
		for ( x = 0;x < numParticles[0].amountElectron;x++ ) {
			
			calculate_force( *index , x , &current );
			calculate_acceleration( PROTON_MASS , &current );
			calculate_velocity( *index , x , time - initialTime , &current );
			calculate_displacement( *index , x , time - initialTime , &current );
			
		
		}
		current.type = PR_PR;
		
		for ( x = 0;x < numParticles[0].amountProton;x++ ) {
			
			if ( x == *index ) {
				
				continue;
			
			} else {
			
				calculate_force( *index , x , &current );
				calculate_acceleration( PROTON_MASS , &current );
				calculate_velocity( *index , x , time - initialTime , &current );
				calculate_displacement( *index , x , time - initialTime , &current );
				
			}
			
		}
		
		temp = new_position(protonLocations[*index].x , current.displacementX , protonLocations[*index].radius);
		if ( current.displacementX != temp ) {
			
			protonLocations[*index].x = temp;
			boundary(&current, XY);
		
		} else {
		
			protonLocations[*index].x += current.displacementX;
		
		}
		
		temp = new_position(protonLocations[*index].y , current.displacementY , protonLocations[*index].radius);
		if ( current.displacementY !=  temp ) {
		
			protonLocations[*index].y = temp;
			boundary(&current, XY);
			
		} else {
		
			protonLocations[*index].y += current.displacementY;
			
		}
		
		temp = new_position(protonLocations[*index].z , current.displacementZ , protonLocations[*index].radius);
		if ( current.displacementZ != temp ) {
		
			protonLocations[*index].z = temp;
			boundary(&current, YZ);
			
		} else {
		
			protonLocations[*index].z += current.displacementZ;
		
		}
		
		
		protonLocations[*index].radius = ( 1 + protonLocations[*index].z ) / 22.22;
		
		initialTime = time;
		time += get_system_time();
		nanosleep( &hold , NULL );
	
	}

	pthread_exit(EXIT_SUCCESS);
	
}

void get_random_location(int index, int type) {
	
	switch (type) {
	
		case ELECTRON:
		
			electronLocations[index].x = get_float() - get_float();
			electronLocations[index].y = get_float() - get_float();
			electronLocations[index].z = get_float() - get_float();
			
		break;
		case PROTON:
		
			protonLocations[index].x = get_float() - get_float();
			protonLocations[index].y = get_float() - get_float();
			protonLocations[index].z = get_float() - get_float();
		
		break;
		
	}
	
}

void compare_locations( int index, int type ) {
	
	int x;
	
	int done = 0;
	
	switch (type) {
		
		case ELECTRON:
		
			for (x = 0; x < index; x++) {
			
				if ( electronLocations[index].x == electronLocations[x].x && 
					 electronLocations[index].y == electronLocations[x].y &&
					 electronLocations[index].z == electronLocations[x].z ) {
					
				
					 get_random_location( index , ELECTRON);
					 x = 0;
				
				}
				
			}
			
		break;
		case PROTON:
			
			while (done == 0) {
				
				done = 1;
				for (x = 0; x < numParticles->amountElectron; x++) {
			
					if ( protonLocations[index].x == electronLocations[x].x && 
						 protonLocations[index].y == electronLocations[x].y &&
						 protonLocations[index].z == electronLocations[x].z ) {
					
				
						get_random_location( index , PROTON);
						x = 0;
				
					}
				
				}
				for ( x = 0; x < index;x++ ) {
			
					if ( protonLocations[index].x == protonLocations[x].x &&
						 protonLocations[index].y == protonLocations[x].y &&
						 protonLocations[index].z == protonLocations[x].z) {
					 
							get_random_location( index , PROTON );	
							x = 0;
							
							done = 0;
				
					}
			
				}
				
			}
		
		break;
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
	
	
	switch ( this->type ) {
		
		case EL_EL:
		
			if ( electronLocations[index1].x > electronLocations[index2].x && electronLocations[index1].x < 1.0) {
			
				this->displacementX += ( metres_velocityTime( this->velocityX , time ) / scale );
		
			} else if ( electronLocations[index1].x == electronLocations[index2].x ) {
			
			
		
			} else {
			
				this->displacementX -=  ( metres_velocityTime( this->velocityX , time ) / scale );
		
			}
		
			if ( electronLocations[index1].y > electronLocations[index2].y && electronLocations[index1].y < 1.0 ) {
			
				this->displacementY += ( metres_velocityTime( this->velocityY , time ) / scale );
		
			} else if ( electronLocations[index1].y == electronLocations[index2].y ) {
			
				
		
			} else {
			
				this->displacementY -= ( metres_velocityTime( this->velocityY , time ) / scale );
				
			}
		
			if (electronLocations[index1].z > electronLocations[index2].z && electronLocations[index1].z < 1.0 ) {
			
				this->displacementZ += ( metres_velocityTime( this->velocityZ , time ) / scale );
		
			} else if ( electronLocations[index1].z == electronLocations[index2].z ) {
			
			
		
			} else {
			
				this->displacementZ -= ( metres_velocityTime( this->velocityZ , time ) / scale );
			
			}
			
		break;
		
		case EL_PR:
		
			if ( electronLocations[index1].x > protonLocations[index2].x && electronLocations[index1].x > -1.0 ) {
				
				this->displacementX += ( metres_velocityTime( this->velocityX , time ) );
			
			} else if ( electronLocations[index1].x == protonLocations[index2].x ) {
				
				
				
			} else {
				
				this->displacementX -= ( metres_velocityTime( this->velocityX , time ) );
			
			}
			
			if ( electronLocations[index1].y > protonLocations[index2].y && electronLocations[index1].y > -1.0 ) {
				
				this->displacementY += ( metres_velocityTime( this->velocityY , time ) );
			
			} else if ( electronLocations[index1].y == protonLocations[index2].y ) {
				
			
				
			} else {
				
				this->displacementY -= ( metres_velocityTime( this->velocityY , time ) );
			
			}
			
			if ( electronLocations[index1].z > protonLocations[index2].z && electronLocations[index1].z > -1.0 ) {
				
				this->displacementZ += ( metres_velocityTime( this->velocityZ , time ) );
			
			} else if ( electronLocations[index1].z == protonLocations[index2].z ) {
				
				
			
			} else {
				
				this->displacementZ -= ( metres_velocityTime( this->velocityZ , time ) );
			
			}
		
		break;
		
		case PR_PR:
		
			if ( protonLocations[index1].x > protonLocations[index2].x && protonLocations[index1].x < 1.0 ) {
			
				this->displacementX += ( metres_velocityTime( this->velocityX , time ) / scale );
		
			} else if ( protonLocations[index1].x == protonLocations[index2].x ) {
			
			
		
			} else {
			
				this->displacementX -=  ( metres_velocityTime( this->velocityX , time ) / scale );
		
			}
		
			if ( protonLocations[index1].y > protonLocations[index2].y && protonLocations[index1].y < 1.0 ) {
			
				this->displacementY += ( metres_velocityTime( this->velocityY , time ) / scale );
		
			} else if ( protonLocations[index1].y == protonLocations[index2].y ) {
			
				
		
			} else {
			
				this->displacementY -= ( metres_velocityTime( this->velocityY , time ) / scale );
				
			}
		
			if ( protonLocations[index1].z > protonLocations[index2].z && protonLocations[index1].z < 1.0 ) {
			
				this->displacementZ += ( metres_velocityTime( this->velocityZ , time ) / scale );
		
			} else if ( protonLocations[index1].z == protonLocations[index2].z ) {
			
			
		
			} else {
			
				this->displacementZ -= ( metres_velocityTime( this->velocityZ , time ) / scale );
			
			}
		
		break;
		
		case PR_EL:
		
			if ( protonLocations[index1].x > electronLocations[index2].x && protonLocations[index1].x > -1.0 ) {
				
				this->displacementX += ( metres_velocityTime( this->velocityX , time ) );
			
			} else if ( protonLocations[index1].x == electronLocations[index2].x ) {
				
				
				
			} else {
				
				this->displacementX -= ( metres_velocityTime( this->velocityX , time ) );
			
			}
			
			if ( protonLocations[index1].y > electronLocations[index2].y && protonLocations[index1].y > -1.0 ) {
				
				this->displacementY += ( metres_velocityTime( this->velocityY , time ) );
			
			} else if ( protonLocations[index1].y == electronLocations[index2].y ) {
				
			
				
			} else {
				
				this->displacementY -= ( metres_velocityTime( this->velocityY , time ) );
			
			}
			
			if ( protonLocations[index1].z > electronLocations[index2].z && protonLocations[index1].z > -1.0 ) {
				
				this->displacementZ += ( metres_velocityTime( this->velocityZ , time ) );
			
			} else if ( protonLocations[index1].z == electronLocations[index2].z ) {
				
				
			
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
