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

#ifndef NEWTONIAN_H
#define NEWTONIAN_H

#include <stdio.h>
#include <math.h>

#include "constants.h"

#define XY 0
#define YZ 1

#define FORCE        0
#define ACCELERATION 1
#define VELOCITY     2

struct movement {
	
	int type;
	
	long double velocity , velocityX , velocityY , velocityZ;
	long double initialVelocityX , initialVelocityY , initialVelocityZ;
	long double acceleration , accelerationX , accelerationY , accelerationZ;
	long double force , forceX , forceY , forceZ;
	long double displacementX , displacementY , displacementZ;

};

long double G_force_massMass(long double , long double, long double);

long double momentum_massVelocity( long double  , long double );
long double velocity_accelerationTime( long double , long double );

long double force_massAcceleration( long double ,long double );
long double mass_forceAcceleration( long double , long double );
long double acceleration_forceMass( long double , long double );

long double metres_velocityTime( long double , long double );
long double velocity_metresTime( long double , long double );
long double time_velocityMetres( long double , long double );

void boundary(struct movement *current, int type);

#endif
