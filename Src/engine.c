/*
 * engine.c
 * 
 * Copyright 2014 Michael Davenport <Davenport.physics@gmail.com>
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

#include "engine.h"

#define SLICES 30.f

void drawParticles( int *readyElectron , int *readyProton );
void drawCircle(float radius, float triangles);
void drawCircle_v2(float radius);
void drawSphere(double r, int lats, int longs);

static float *coscalc;
static float *sincalc;

SDL_Window* Window;

int engine_init() {
	
	float degrees = (360 / SLICES);
	float current = 0;
	
	coscalc = ( float * )malloc( ( SLICES + 1 ) * sizeof( float ));
	sincalc = ( float * )malloc( ( SLICES + 1 ) * sizeof( float ));
	
	int x;
	for ( x = 0; x < (int)SLICES; x++ ) {
		
		coscalc[x] = cos( current * ( M_PI / 180.f ) );
		sincalc[x] = sin( current * ( M_PI / 180.f ) );
		current += degrees;
		
	}
	coscalc[(int)SLICES] = cos(0);
	sincalc[(int)SLICES] = sin(0);
	
	SDL_Init(SDL_INIT_VIDEO);
	
	Window = SDL_CreateWindow( 
			"Physics", 
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			800,
			800,
			SDL_WINDOW_OPENGL);

	return 0;

}
void engine_quit() {
	
	free(coscalc);
	free(sincalc);
	
	coscalc = NULL;
	sincalc = NULL;
	
	SDL_DestroyWindow( Window );
	SDL_Quit();
	
}

void engine_run(struct enginevars *vars, int *types, enum engine_runtime runtime) {
	
	int x;
	
	struct timespec hold;
	hold.tv_sec = 0;
	hold.tv_nsec = 1666666;
	
	SDL_GLContext glcontext = SDL_GL_CreateContext(Window);
	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	
	if (runtime == NORMAL) {
	
		while (systemFinished == 0) {
		
			glClear(GL_COLOR_BUFFER_BIT);
			glLoadIdentity();
		
			for ( x = 0; x < 3; x++ ) {
		
				switch (types[x]) {
				
					case 1: drawParticles( &vars->readyElectron , &vars->readyProton ); break;
			
				}
		
			}
			SDL_GL_SwapWindow(Window);
			nanosleep( &hold , NULL );
		
		}
	} else {
	
		while (systemFinished == 0) {
		
			glClear(GL_COLOR_BUFFER_BIT);
			glLoadIdentity();
			
			SDL_GL_SwapWindow(Window);
			nanosleep( &hold , NULL );
			
		}
		
	}
	SDL_GL_DeleteContext(glcontext);
	
}
void drawParticles( int *readyElectron , int *readyProton ) {
	
	int x;
	
	for (x = 0;x < *readyElectron; x++) {
		
		glPushMatrix();
			
			glColor3f( 0.0f,0.0f,0.0f );
			glTranslatef( electronLocations[x].x , electronLocations[x].y , electronLocations[x].z);
			//drawCircle( electronLocations[x].radius, 30);
			drawCircle_v2(electronLocations[x].radius);
	
		glPopMatrix();
	
	}
	for (x = 0;x < *readyProton; x++) {
		
		glPushMatrix();
		
			glColor3f( 0.0f,0.0f,1.0f );
			glTranslatef( protonLocations[x].x , protonLocations[x].y , protonLocations[x].z );
			//drawCircle( protonLocations[x].radius, 30);
			drawCircle_v2(protonLocations[x].radius);
		
		glPopMatrix();
	
	}
	
}
void drawCircle(float radius, float triangles) {
	
	float degrees = ( 360 / triangles );
	float current = 0;
	
	int x;
	
	glBegin(GL_TRIANGLES);

		for ( x = 0; x < (int)triangles; x++) {
			
			glVertex3f( radius * cos( current * (M_PI/180) ), radius * sin( current * (M_PI/180) ) , 0.0f );
			current += degrees;
			glVertex3f( radius * cos( current * ( M_PI / 180 ) ), radius * sin( current * (M_PI/180) ) , 0.0f );
			glVertex3f(0.0f, 0.0f, 0.0f);
		
		}
	
	glEnd();
	
}
void drawCircle_v2(float radius) {
	
	int x;
	
	glBegin(GL_TRIANGLES);
	
	for ( x = 0; x < (int)SLICES; x++ ) {
		
		glVertex3f(radius * coscalc[x] , radius * sincalc[x] , 0.0f);
		glVertex3f( radius * coscalc[(x+1)] , radius * sincalc[x] , 0.0f );
		glVertex3f( 0.0f , 0.0f , 0.0f );
	
	}
	
	glEnd();

}
 void drawSphere(double r, int lats, int longs) {
	 
    int i, j;
    
    for(i = 0; i <= lats; i++) {
		
       double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
       double z0  = sin(lat0);
       double zr0 =  cos(lat0);

       double lat1 = M_PI * (-0.5 + (double) i / lats);
       double z1 = sin(lat1);
       double zr1 = cos(lat1);

       glBegin(GL_QUAD_STRIP);
       
       for(j = 0; j <= longs; j++) {
		   
           double lng = 2 * M_PI * (double) (j - 1) / longs;
           double x = cos(lng);
           double y = sin(lng);

           glNormal3f(x * zr0, y * zr0, z0);
           glVertex3f(x * zr0, y * zr0, z0);
           glNormal3f(x * zr1, y * zr1, z1);
           glVertex3f(x * zr1, y * zr1, z1);
           
       }
       
       glEnd();
       
   }
   
 }

void *engine_event(void *n) {
	
	SDL_Event event;
	
	while ( systemFinished == CONTINUE ) {
		
		while ( SDL_PollEvent( &event ) ) {
			
			if (event.type == SDL_KEYDOWN) {
			
				if (event.key.keysym.sym == 'q') {
				
					systemFinished = FINISH;
					
				}
				
			} else if (event.type == SDL_QUIT) {
				
				systemFinished = FINISH;
				
			}
			
		}
		
	}
	
	pthread_exit(EXIT_SUCCESS);
	
}
