/*
 * newtonian.c
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


#include "newtonian.h"

inline long double momentum_massVelocity(long double M , long double V) {
	
	return M * V;
	
}

inline long double velocity_accelerationTime( long double A , long double T) {
	
	return A * T;

}

inline long double force_massAcceleration(long double M , long double A) {
	
	return M * A;
	
}

inline long double mass_forceAcceleration( long double F , long double A ) {
	
	return F / A;

}

inline long double acceleration_forceMass( long double F , long double M ) {
	
	return F / M;

}

inline long double metres_velocityTime( long double V , long double T ) {
	
	return V * T;
	
}

inline long double velocity_metresTime( long double M , long double T ) {
	
	return M / T;

}

inline long double time_velocityMetres( long double V , long double M ) {
	
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
