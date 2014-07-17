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

void drawCircle(float radius, float triangles);
void drawSphere(double r, int lats, int longs);


SDL_Window* Window;

int engine_init() {
	
	SDL_Init (SDL_INIT_VIDEO);
	
	Window = SDL_CreateWindow( 
			"Physics", 
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			800,
			800,
			SDL_WINDOW_OPENGL);

	return 0;

}
void engine_run(int *readyElectron, int *readyProton) {
	
	int x;
	
	struct timespec *hold = ( struct timespec *)malloc( sizeof ( struct timespec ) );
	hold[0].tv_sec = 0;
	hold[0].tv_nsec = 1666666;
	
	SDL_GLContext glcontext = SDL_GL_CreateContext(Window);
	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	
	while (systemFinished == 0) {
		
		glClear(GL_COLOR_BUFFER_BIT);
		glLoadIdentity();
		
		for (x = 0;x < *readyElectron; x++) {
		
			glPushMatrix();
			
				glColor3f( 0.0f,0.0f,0.0f );
				glTranslatef( electronLocations[x].x , electronLocations[x].y , electronLocations[x].z);
				drawCircle( (1 + electronLocations[x].z)/33.33, 30);
	
			glPopMatrix();
	
		}
		for (x = 0;x < *readyProton; x++) {
		
			glPushMatrix();
		
				glColor3f( 0.0f,0.0f,1.0f );
				glTranslatef( protonLocations[x].x , protonLocations[x].y , protonLocations[x].z );
				drawCircle( (1 + protonLocations[x].z)/33.33, 30);
		
			glPopMatrix();
	
		}
		SDL_GL_SwapWindow(Window);
		nanosleep( hold , NULL );
		
	}
	SDL_GL_DeleteContext(glcontext);
	
}
void drawCircle(float radius, float triangles) {
	
	float degrees = ( 360 / triangles);
	float current = 0;
	
	int x;
	
	glBegin(GL_TRIANGLES);

		for ( x = 0; x < (int)triangles; x++) {
			
			glVertex3f(radius*cos(current*(M_PI/180)), radius*sin(current*(M_PI/180)) , 0.0f );
			current += degrees;
			glVertex3f(radius*cos(current*(M_PI/180)), radius*sin(current*(M_PI/180)) , 0.0f );
			glVertex3f(0.0f, 0.0f, 0.0f);
		
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

void engine_quit() {
	
	SDL_DestroyWindow( Window );
	SDL_Quit();
	
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
