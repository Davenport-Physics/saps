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

#ifndef ENGINE_H
#define ENGINE_H

#include "SDL2/SDL.h"
#include "GL/gl.h"

#include <pthread.h>
#include <math.h>

#include "particles.h"
#include "systemtime.h"

enum engine_runtime {

	NORMAL = 0,
	DEBUG
	
};

struct enginevars {
	
	int readyElectron;
	int readyProton;
	
};

int engine_init();
void engine_run(struct enginevars *vars, int *types, int typeLength , enum engine_runtime runtime);
void engine_quit();

void *engine_event(void *n);

#endif


