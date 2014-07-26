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

#include "systemtime.h"

int systemFinished;

static long double systemTime;


/*
 * This will probably be replaced by another function that is much more
 * efficient and accurate. However for the moment, this clock is used to
 * determine the velocity of a particle with respect to acceleration and 
 * any displacement.
 * 
 * */
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
