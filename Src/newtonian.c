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


#include "newtonian.h"

<<<<<<< HEAD
long double G_force_massMass(long double mass1, long double mass2, long double radius) {
	
	return ( GRAVITATIONAL_CONSTANT * mass1 * mass2 ) / ( radius * radius );
	
}

=======
>>>>>>> 3505bf6e66286cc7d964b0bf5e4c0891f3afe216
long double momentum_massVelocity(long double M , long double V) {
	
	return M * V;
	
}

long double velocity_accelerationTime( long double A , long double T) {
	
	return A * T;

}

long double force_massAcceleration(long double M , long double A) {
	
	return M * A;
	
}

long double mass_forceAcceleration( long double F , long double A ) {
	
	return F / A;

}

long double acceleration_forceMass( long double F , long double M ) {
	
	return F / M;

}

long double metres_velocityTime( long double V , long double T ) {
	
	return V * T;
	
}

long double velocity_metresTime( long double M , long double T ) {
	
	return M / T;

}

long double time_velocityMetres( long double V , long double M ) {
	
	return V / M;

}

void boundary(struct movement *current , int type) {
	
	long double *x;
	long double *y;
	
	long double xytheta;
	
	if (type == XY) {
	
		x = &current->velocityX;
		y = &current->velocityY;
	
	} else {
	
		x = &current->velocityY;
		y = &current->velocityZ;
		
	}
	
	if ( *x >= 0 && *y >= 0 ) {
	
		xytheta = atan(*y / *x) + M_PI/2;
		
	} else if ( *x <= 0 && *y >= 0) {
	
		xytheta = atan(*y / *x) + M_PI/2;
		
	} else if ( *x <= 0 && *y <= 0 ) {
		
		xytheta = atan(*y / *x) - M_PI/2;
	
	} else {
	
		xytheta =  ((3 * M_PI)/2) * atan(*y / *x);
		
	}
	
	*x = *x * cos(xytheta);
	*y = *y * sin(xytheta);
	
}
