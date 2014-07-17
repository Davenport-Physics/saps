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
