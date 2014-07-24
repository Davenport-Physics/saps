/*
 * systemtime.c
 * 
 * Copyright 2013 Michael Davenport <Davenport.physics@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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


#include "systemtime.h"

static long double systemTime;

void *system_clock( void *n ) {
	
	systemTime = 0.00000000000000000000;
	
	struct timespec time;
	time.tv_sec = 0;
	time.tv_nsec = 1000000;
	
	while ( systemFinished == CONTINUE ) {
		
		nanosleep( &time, NULL );
		systemTime += 0.00100000000000000000;
	
	}
	
	
	pthread_exit(EXIT_SUCCESS);

}

long double get_system_time() {
	
	return systemTime;

}
