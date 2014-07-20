/*
 * newtonian.h
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

#ifndef NEWTONIAN_H
#define NEWTONIAN_H

#include <stdio.h>
#include <math.h>

struct movement {
	
	int type;
	
	long double velocity , velocityX , velocityY , velocityZ;
	long double initialVelocityX , initialVelocityY , initialVelocityZ;
	long double acceleration , accelerationX , accelerationY , accelerationZ;
	long double force , forceX , forceY , forceZ;
	long double displacementX , displacementY , displacementZ;

};


inline long double momentum_massVelocity( long double  , long double );
inline long double velocity_accelerationTime( long double , long double );

inline long double force_massAcceleration( long double ,long double );
inline long double mass_forceAcceleration( long double , long double );
inline long double acceleration_forceMass( long double , long double );

inline long double metres_velocityTime( long double , long double );
inline long double velocity_metresTime( long double , long double );
inline long double time_velocityMetres( long double , long double );

void boundary_xy(struct movement *current);
void boundary_yz(struct movement *current);

#endif
